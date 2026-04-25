// ============================================================================
// MULTI-PLANETARY ORBITAL COLLISION PREDICTION SYSTEM
// ============================================================================
// Real orbital mechanics for Earth, Mars, Venus, Jupiter, and Moon
// Built for engineering analysis and space traffic management

// Gravitational constant (m^3 kg^-1 s^-2)
const G = 6.674e-11;

// Planet data - accurate mass, radius, colors
const PLANETS = {
    earth: {
        name: 'Earth',
        mass: 5.972e24,      // kg
        radius: 6371,        // km
        color: 0x1a4a8a,     // ocean blue
        atmColor: 0x2a5a9a
    },
    mars: {
        name: 'Mars',
        mass: 6.417e23,      // kg
        radius: 3389.5,      // km
        color: 0xcd5c5c,     // rust red
        atmColor: 0xdd6c6c
    },
    venus: {
        name: 'Venus',
        mass: 4.867e24,      // kg
        radius: 6051.8,      // km
        color: 0xffa500,     // orange
        atmColor: 0xffb520
    },
    jupiter: {
        name: 'Jupiter',
        mass: 1.898e27,      // kg
        radius: 69911,       // km
        color: 0xdaa520,     // goldenrod
        atmColor: 0xeab530
    },
    moon: {
        name: 'Moon',
        mass: 7.342e22,      // kg
        radius: 1737.4,      // km
        color: 0x8a8a8a,     // gray
        atmColor: 0x9a9a9a
    }
};

// Current state
let currentPlanet = 'earth';
let scene, camera, renderer, controls;
let planetMesh, satellites = [], debris = [];
let running = false;
let time = 0;
let speed = 1.0;
let collisions = [];
let particles = [];
let dangerZoneMesh = null;
let showTrails = true;
let showDangerZone = false;

// Configurable parameters
let altMin = 400;
let altMax = 30000;
let inclinationRange = 180;
let eccentricity = 0.0;
let collisionThreshold = 3000;  // km
let predictionWindow = 5;        // minutes

// ============================================================================
// INITIALIZATION
// ============================================================================

function init() {
    setupScene();
    createPlanet();
    createStars();
    createDangerZone();
    createObjects();
    setupControls();
    updatePlanetInfo();
    updatePhysicsCalculations();
    animate();
    updateUI();
}

// ============================================================================
// SCENE SETUP
// ============================================================================

function setupScene() {
    const container = document.querySelector('.canvas-section');
    const canvas = document.getElementById('canvas');

    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x000000);

    camera = new THREE.PerspectiveCamera(
        50,
        container.clientWidth / container.clientHeight,
        1,
        50000
    );
    camera.position.set(400, 250, 400);

    renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
    renderer.setSize(container.clientWidth, container.clientHeight);
    renderer.setPixelRatio(window.devicePixelRatio);

    // OrbitControls - full mouse interaction
    controls = new THREE.OrbitControls(camera, canvas);
    controls.enableDamping = true;
    controls.dampingFactor = 0.05;
    controls.minDistance = 50;
    controls.maxDistance = 5000;

    // Window resize handling
    window.addEventListener('resize', () => {
        camera.aspect = container.clientWidth / container.clientHeight;
        camera.updateProjectionMatrix();
        renderer.setSize(container.clientWidth, container.clientHeight);
    });

    // Lighting setup
    scene.add(new THREE.AmbientLight(0x404040, 1));
    const sun = new THREE.DirectionalLight(0xffffff, 1.5);
    sun.position.set(500, 300, 400);
    scene.add(sun);
}

function createPlanet() {
    // Remove old planet if exists
    if (planetMesh) {
        scene.remove(planetMesh);
    }

    const planetData = PLANETS[currentPlanet];
    const radius = planetData.radius * 0.01;

    const geo = new THREE.SphereGeometry(radius, 64, 64);
    const mat = new THREE.MeshPhongMaterial({
        color: planetData.color,
        shininess: 15
    });
    planetMesh = new THREE.Mesh(geo, mat);
    scene.add(planetMesh);

    // Equator reference line
    const points = [];
    for (let i = 0; i <= 128; i++) {
        const angle = (i / 128) * Math.PI * 2;
        points.push(new THREE.Vector3(
            Math.cos(angle) * radius,
            0,
            Math.sin(angle) * radius
        ));
    }
    const lineGeo = new THREE.BufferGeometry().setFromPoints(points);
    const lineMat = new THREE.LineBasicMaterial({
        color: 0x444444,
        transparent: true,
        opacity: 0.3
    });
    const line = new THREE.Line(lineGeo, lineMat);
    planetMesh.add(line);
}

function createStars() {
    const geo = new THREE.BufferGeometry();
    const verts = [];
    for (let i = 0; i < 4000; i++) {
        verts.push(
            (Math.random() - 0.5) * 8000,
            (Math.random() - 0.5) * 8000,
            (Math.random() - 0.5) * 8000
        );
    }
    geo.setAttribute('position', new THREE.Float32BufferAttribute(verts, 3));
    const mat = new THREE.PointsMaterial({
        color: 0xffffff,
        size: 1.5,
        transparent: true,
        opacity: 0.6
    });
    scene.add(new THREE.Points(geo, mat));
}

// ============================================================================
// ORBITAL OBJECTS
// ============================================================================

function createObjects() {
    clearObjects();

    const satCount = parseInt(document.getElementById('sat-slider').value);
    const debCount = parseInt(document.getElementById('deb-slider').value);

    // Create satellites with configured parameters
    for (let i = 0; i < satCount; i++) {
        satellites.push(createSatellite());
    }

    // Create debris
    for (let i = 0; i < debCount; i++) {
        debris.push(createDebris());
    }

    updateUI();
    updatePhysicsCalculations();
}

function createSatellite() {
    const planetData = PLANETS[currentPlanet];

    // Use user-configured altitude range
    const altitude = altMin + Math.random() * (altMax - altMin);
    const radius = (planetData.radius + altitude) * 0.01;

    // Use user-configured inclination range
    const maxIncl = inclinationRange * Math.PI / 180;
    const inclination = (Math.random() * 2 - 1) * (maxIncl / 2);

    const raan = Math.random() * Math.PI * 2;
    const angle = Math.random() * Math.PI * 2;

    // Satellite mesh (white dot with glow)
    const geo = new THREE.SphereGeometry(0.8, 8, 8);
    const mat = new THREE.MeshBasicMaterial({ color: 0xffffff });
    const mesh = new THREE.Mesh(geo, mat);
    scene.add(mesh);

    // Trail line for satellite
    const trailGeo = new THREE.BufferGeometry();
    const trailPositions = new Float32Array(100 * 3); // 100 points
    trailGeo.setAttribute('position', new THREE.BufferAttribute(trailPositions, 3));
    const trailMat = new THREE.LineBasicMaterial({
        color: 0x00ffff,
        transparent: true,
        opacity: 0.3,
        linewidth: 1
    });
    const trailLine = new THREE.Line(trailGeo, trailMat);
    trailLine.visible = showTrails;
    scene.add(trailLine);

    // Orbital path line
    const orbitPoints = [];
    for (let i = 0; i <= 64; i++) {
        const a = (i / 64) * Math.PI * 2;
        const x = radius * Math.cos(a);
        const z = radius * Math.sin(a);
        const y1 = -z * Math.sin(inclination);
        const z1 = z * Math.cos(inclination);
        const x2 = x * Math.cos(raan) - y1 * Math.sin(raan);
        const y2 = x * Math.sin(raan) + y1 * Math.cos(raan);
        orbitPoints.push(new THREE.Vector3(x2, y2, z1));
    }
    const orbitGeo = new THREE.BufferGeometry().setFromPoints(orbitPoints);
    const orbitMat = new THREE.LineBasicMaterial({
        color: 0x333333,
        transparent: true,
        opacity: 0.15
    });
    const orbitLine = new THREE.Line(orbitGeo, orbitMat);
    scene.add(orbitLine);

    // Calculate orbital velocity using vis-viva equation
    const r = (planetData.radius + altitude) * 1000;  // meters
    const velocity = Math.sqrt((G * planetData.mass) / r) / 1000;  // km/s

    // Calculate orbital period
    const period = (2 * Math.PI * Math.sqrt(Math.pow(r, 3) / (G * planetData.mass))) / 60;  // minutes
    const angularVel = (2 * Math.PI) / (period * 60);  // rad/s

    return {
        type: 'sat',
        mesh,
        orbitLine,
        trailLine,
        trailHistory: [],
        radius,
        inclination,
        raan,
        angle,
        velocity,
        period,
        angularVel,
        altitude
    };
}

function createDebris() {
    const planetData = PLANETS[currentPlanet];

    // Debris can be at wider altitude range
    const altitude = 200 + Math.random() * (altMax * 1.3);
    const radius = (planetData.radius + altitude) * 0.01;
    const inclination = (Math.random() * 180 - 90) * Math.PI / 180;
    const raan = Math.random() * Math.PI * 2;
    const angle = Math.random() * Math.PI * 2;

    // Debris mesh (red dot, smaller)
    const geo = new THREE.SphereGeometry(0.4, 6, 6);
    const mat = new THREE.MeshBasicMaterial({ color: 0xcc4444 });
    const mesh = new THREE.Mesh(geo, mat);
    scene.add(mesh);

    // Physics calculations
    const r = (planetData.radius + altitude) * 1000;
    const velocity = Math.sqrt((G * planetData.mass) / r) / 1000;
    const period = (2 * Math.PI * Math.sqrt(Math.pow(r, 3) / (G * planetData.mass))) / 60;
    const angularVel = (2 * Math.PI) / (period * 60);

    return {
        type: 'deb',
        mesh,
        radius,
        inclination,
        raan,
        angle,
        velocity,
        period,
        angularVel,
        altitude
    };
}

function clearObjects() {
    satellites.forEach(s => {
        scene.remove(s.mesh);
        if (s.orbitLine) scene.remove(s.orbitLine);
        if (s.trailLine) scene.remove(s.trailLine);
    });
    debris.forEach(d => scene.remove(d.mesh));
    satellites = [];
    debris = [];

    // Remove collision visualization lines
    scene.children.filter(c => c.userData.collision).forEach(c => scene.remove(c));

    // Remove particles
    particles.forEach(p => scene.remove(p.mesh));
    particles = [];
}

function updatePosition(obj) {
    // Calculate 3D position from Keplerian elements
    const x = obj.radius * Math.cos(obj.angle);
    const z = obj.radius * Math.sin(obj.angle);
    const y1 = -z * Math.sin(obj.inclination);
    const z1 = z * Math.cos(obj.inclination);
    const x2 = x * Math.cos(obj.raan) - y1 * Math.sin(obj.raan);
    const y2 = x * Math.sin(obj.raan) + y1 * Math.cos(obj.raan);
    obj.mesh.position.set(x2, y2, z1);

    // Update trail for satellites
    if (obj.type === 'sat' && obj.trailLine) {
        obj.trailHistory.push(obj.mesh.position.clone());
        if (obj.trailHistory.length > 100) {
            obj.trailHistory.shift();
        }
        const positions = obj.trailLine.geometry.attributes.position;
        for (let i = 0; i < 100; i++) {
            if (i < obj.trailHistory.length) {
                const p = obj.trailHistory[i];
                positions.setXYZ(i, p.x, p.y, p.z);
            } else {
                positions.setXYZ(i, x2, y2, z1);
            }
        }
        positions.needsUpdate = true;
    }
}

// ============================================================================
// PARTICLE EXPLOSION SYSTEM
// ============================================================================

function createExplosion(position) {
    const particleCount = 30;
    for (let i = 0; i < particleCount; i++) {
        const geo = new THREE.SphereGeometry(0.3, 4, 4);
        const mat = new THREE.MeshBasicMaterial({
            color: Math.random() < 0.5 ? 0xff6600 : 0xffff00,
            transparent: true,
            opacity: 1
        });
        const mesh = new THREE.Mesh(geo, mat);
        mesh.position.copy(position);
        scene.add(mesh);

        const velocity = new THREE.Vector3(
            (Math.random() - 0.5) * 20,
            (Math.random() - 0.5) * 20,
            (Math.random() - 0.5) * 20
        );

        particles.push({
            mesh,
            velocity,
            life: 1.0
        });
    }
}

function updateParticles(dt) {
    for (let i = particles.length - 1; i >= 0; i--) {
        const p = particles[i];
        p.mesh.position.add(p.velocity.clone().multiplyScalar(dt));
        p.life -= dt * 2;
        p.mesh.material.opacity = Math.max(0, p.life);

        if (p.life <= 0) {
            scene.remove(p.mesh);
            particles.splice(i, 1);
        }
    }
}

// ============================================================================
// COLLISION DETECTION
// ============================================================================

function detectCollisions() {
    collisions = [];

    // Clear old collision lines
    scene.children.filter(c => c.userData.collision).forEach(c => scene.remove(c));

    // Reset satellite colors
    satellites.forEach(s => s.mesh.material.color.setHex(0xffffff));

    // Check all satellite-object pairs
    const allObjects = [...satellites, ...debris];
    const thresholdScene = collisionThreshold * 0.01;  // convert km to scene units

    for (let i = 0; i < satellites.length; i++) {
        for (let j = 0; j < allObjects.length; j++) {
            if (i === j && allObjects[j].type === 'sat') continue;

            const obj1 = satellites[i];
            const obj2 = allObjects[j];
            const dist = obj1.mesh.position.distanceTo(obj2.mesh.position);

            // Collision warning if within threshold
            if (dist < thresholdScene) {
                const distKm = dist / 0.01;  // convert scene units to km

                collisions.push({
                    obj1,
                    obj2,
                    dist: distKm
                });

                // ACTUAL COLLISION - create explosion!
                if (distKm < 5 && !obj1.exploded && !obj2.exploded) {
                    const midPoint = obj1.mesh.position.clone().add(obj2.mesh.position).multiplyScalar(0.5);
                    createExplosion(midPoint);
                    obj1.exploded = true;
                    obj2.exploded = true;
                }

                // Mark satellite RED for collision warning
                obj1.mesh.material.color.setHex(0xff0000);

                // Draw bright red line between objects
                const points = [obj1.mesh.position.clone(), obj2.mesh.position.clone()];
                const geo = new THREE.BufferGeometry().setFromPoints(points);
                const mat = new THREE.LineBasicMaterial({
                    color: 0xff0000,
                    linewidth: 3,
                    transparent: true,
                    opacity: 0.8
                });
                const line = new THREE.Line(geo, mat);
                line.userData.collision = true;
                scene.add(line);
            }
        }
    }

    updateAlerts();
}

// ============================================================================
// DANGER ZONE VISUALIZATION
// ============================================================================

function createDangerZone() {
    if (dangerZoneMesh) {
        scene.remove(dangerZoneMesh);
    }

    const radius = collisionThreshold * 0.01;
    const geo = new THREE.SphereGeometry(radius, 32, 32);
    const mat = new THREE.MeshBasicMaterial({
        color: 0xff0000,
        transparent: true,
        opacity: 0.05,
        wireframe: true
    });
    dangerZoneMesh = new THREE.Mesh(geo, mat);
    dangerZoneMesh.visible = showDangerZone;
    scene.add(dangerZoneMesh);
}

function toggleDangerZone() {
    showDangerZone = !showDangerZone;
    if (dangerZoneMesh) {
        dangerZoneMesh.visible = showDangerZone;
    }
}

function toggleTrails() {
    showTrails = !showTrails;
    satellites.forEach(s => {
        if (s.trailLine) {
            s.trailLine.visible = showTrails;
        }
    });
}

function updateAlerts() {
    const alertsDiv = document.getElementById('alerts');
    alertsDiv.innerHTML = '';

    if (collisions.length === 0) {
        alertsDiv.innerHTML = '<div class="no-alerts">No active threats</div>';
    } else {
        // Show top 15 collisions
        collisions.slice(0, 15).forEach(col => {
            const div = document.createElement('div');
            div.className = 'alert-item';
            div.textContent = `WARNING: ${col.dist.toFixed(1)} km separation`;
            alertsDiv.appendChild(div);
        });
    }
}

// ============================================================================
// USER INTERFACE UPDATES
// ============================================================================

function updateUI() {
    document.getElementById('sat-count').textContent = satellites.length;
    document.getElementById('deb-count').textContent = debris.length;
    document.getElementById('col-count').textContent = collisions.length;
    document.getElementById('total-count').textContent = satellites.length + debris.length;
}

function updateTime() {
    const hours = Math.floor(time / 3600);
    const mins = Math.floor((time % 3600) / 60);
    const secs = Math.floor(time % 60);
    document.getElementById('time').textContent =
        String(hours).padStart(2, '0') + ':' +
        String(mins).padStart(2, '0') + ':' +
        String(secs).padStart(2, '0');
}

function updatePlanetInfo() {
    const planetData = PLANETS[currentPlanet];

    // Calculate surface gravity: g = GM/r^2
    const r = planetData.radius * 1000;  // meters
    const g = (G * planetData.mass) / (r * r);

    document.getElementById('planet-mass').textContent = planetData.mass.toExponential(3) + ' kg';
    document.getElementById('planet-radius').textContent = planetData.radius.toFixed(1) + ' km';
    document.getElementById('planet-gravity').textContent = g.toFixed(2) + ' m/s²';
}

function updatePhysicsCalculations() {
    const planetData = PLANETS[currentPlanet];

    // Calculate for average altitude
    const avgAlt = (altMin + altMax) / 2;
    const r = (planetData.radius + avgAlt) * 1000;  // meters

    // Orbital velocity: v = sqrt(GM/r)
    const velocity = Math.sqrt((G * planetData.mass) / r) / 1000;  // km/s

    // Orbital period: T = 2π * sqrt(r^3 / GM)
    const period = (2 * Math.PI * Math.sqrt(Math.pow(r, 3) / (G * planetData.mass))) / 60;  // minutes

    // Escape velocity: v_esc = sqrt(2GM/r)
    const escapeVel = Math.sqrt((2 * G * planetData.mass) / r) / 1000;  // km/s

    document.getElementById('calc-velocity').textContent = velocity.toFixed(2) + ' km/s';
    document.getElementById('calc-period').textContent = period.toFixed(1) + ' min';
    document.getElementById('calc-escape').textContent = escapeVel.toFixed(2) + ' km/s';
}

// ============================================================================
// CONTROL HANDLERS
// ============================================================================

function setupControls() {
    // START button
    document.getElementById('btn-start').addEventListener('click', () => {
        running = true;
        document.getElementById('status').textContent = 'RUNNING';
        document.getElementById('status').style.color = '#00ff00';
    });

    // PAUSE button
    document.getElementById('btn-pause').addEventListener('click', () => {
        running = false;
        document.getElementById('status').textContent = 'PAUSED';
        document.getElementById('status').style.color = '#f59e0b';
    });

    // RESET button
    document.getElementById('btn-reset').addEventListener('click', () => {
        running = false;
        time = 0;
        createObjects();
        document.getElementById('status').textContent = 'RESET';
        document.getElementById('status').style.color = '#dc2626';
    });

    // Planet selector
    document.getElementById('planet-select').addEventListener('change', (e) => {
        currentPlanet = e.target.value;
        createPlanet();
        updatePlanetInfo();
        updatePhysicsCalculations();
        if (!running) createObjects();
    });

    // Satellite count slider
    document.getElementById('sat-slider').addEventListener('input', (e) => {
        document.getElementById('sat-val').textContent = e.target.value;
    });
    document.getElementById('sat-slider').addEventListener('change', () => {
        if (!running) createObjects();
    });

    // Debris count slider
    document.getElementById('deb-slider').addEventListener('input', (e) => {
        document.getElementById('deb-val').textContent = e.target.value;
    });
    document.getElementById('deb-slider').addEventListener('change', () => {
        if (!running) createObjects();
    });

    // Altitude min slider
    document.getElementById('alt-min-slider').addEventListener('input', (e) => {
        altMin = parseInt(e.target.value);
        document.getElementById('alt-min-val').textContent = altMin;
        updatePhysicsCalculations();
    });
    document.getElementById('alt-min-slider').addEventListener('change', () => {
        if (!running) createObjects();
    });

    // Altitude max slider
    document.getElementById('alt-max-slider').addEventListener('input', (e) => {
        altMax = parseInt(e.target.value);
        document.getElementById('alt-max-val').textContent = altMax;
        updatePhysicsCalculations();
    });
    document.getElementById('alt-max-slider').addEventListener('change', () => {
        if (!running) createObjects();
    });

    // Inclination range slider
    document.getElementById('incl-slider').addEventListener('input', (e) => {
        inclinationRange = parseInt(e.target.value);
        document.getElementById('incl-val').textContent = inclinationRange;
    });
    document.getElementById('incl-slider').addEventListener('change', () => {
        if (!running) createObjects();
    });

    // Eccentricity slider
    document.getElementById('ecc-slider').addEventListener('input', (e) => {
        eccentricity = parseFloat(e.target.value);
        document.getElementById('ecc-val').textContent = eccentricity.toFixed(2);
    });
    document.getElementById('ecc-slider').addEventListener('change', () => {
        if (!running) createObjects();
    });

    // Collision threshold slider
    document.getElementById('thresh-slider').addEventListener('input', (e) => {
        collisionThreshold = parseInt(e.target.value);
        document.getElementById('thresh-val').textContent = collisionThreshold;
        createDangerZone(); // Update danger zone size
    });

    // Prediction window slider
    document.getElementById('pred-slider').addEventListener('input', (e) => {
        predictionWindow = parseInt(e.target.value);
        document.getElementById('pred-val').textContent = predictionWindow;
    });

    // Speed slider
    document.getElementById('speed-slider').addEventListener('input', (e) => {
        speed = parseFloat(e.target.value);
        document.getElementById('speed-val').textContent = speed.toFixed(1) + 'x';
    });

    // View preset buttons
    document.getElementById('view-1').addEventListener('click', () => {
        camera.position.set(200, 100, 200);
        controls.target.set(0, 0, 0);
        controls.update();
    });

    document.getElementById('view-2').addEventListener('click', () => {
        camera.position.set(400, 250, 400);
        controls.target.set(0, 0, 0);
        controls.update();
    });

    document.getElementById('view-3').addEventListener('click', () => {
        camera.position.set(800, 600, 800);
        controls.target.set(0, 0, 0);
        controls.update();
    });

    // Feature toggle buttons
    const trailsBtn = document.getElementById('toggle-trails');
    if (trailsBtn) {
        trailsBtn.addEventListener('click', () => {
            toggleTrails();
            trailsBtn.textContent = showTrails ? 'TRAILS: ON' : 'TRAILS: OFF';
            trailsBtn.style.background = showTrails ? '#2563eb' : '#1a1a1a';
        });
    }

    const dangerBtn = document.getElementById('toggle-danger');
    if (dangerBtn) {
        dangerBtn.addEventListener('click', () => {
            toggleDangerZone();
            dangerBtn.textContent = showDangerZone ? 'DANGER: ON' : 'DANGER: OFF';
            dangerBtn.style.background = showDangerZone ? '#dc2626' : '#1a1a1a';
        });
    }

    // Preset scenario buttons
    const presets = ['crowded', 'debris', 'geosync', 'leo'];
    presets.forEach(preset => {
        const btn = document.getElementById(`preset-${preset}`);
        if (btn) {
            btn.addEventListener('click', () => loadPreset(preset));
        }
    });
}

// ============================================================================
// ANIMATION LOOP
// ============================================================================

function animate() {
    requestAnimationFrame(animate);

    if (running) {
        const dt = 0.016 * speed;  // time delta adjusted by speed
        time += dt;

        // Rotate planet slowly
        planetMesh.rotation.y += 0.0002 * speed;

        // Update all satellites
        satellites.forEach(s => {
            s.angle += s.angularVel * dt;
            updatePosition(s);
        });

        // Update all debris
        debris.forEach(d => {
            d.angle += d.angularVel * dt;
            updatePosition(d);
        });

        // Update particles
        updateParticles(dt);

        // Run collision detection every 10 frames for performance
        if (Math.floor(time * 60) % 10 === 0) {
            detectCollisions();
        }

        updateTime();
        updateUI();
    }

    controls.update();
    renderer.render(scene, camera);
}

// ============================================================================
// PRESET SCENARIOS
// ============================================================================

function loadPreset(name) {
    running = false;
    document.getElementById('status').textContent = 'LOADING PRESET';

    switch(name) {
        case 'crowded':
            document.getElementById('sat-slider').value = 120;
            document.getElementById('deb-slider').value = 200;
            document.getElementById('alt-min-slider').value = 400;
            document.getElementById('alt-max-slider').value = 2000;
            document.getElementById('thresh-slider').value = 1500;
            break;
        case 'debris':
            document.getElementById('sat-slider').value = 30;
            document.getElementById('deb-slider').value = 300;
            document.getElementById('alt-min-slider').value = 200;
            document.getElementById('alt-max-slider').value = 5000;
            document.getElementById('thresh-slider').value = 2000;
            break;
        case 'geosync':
            document.getElementById('sat-slider').value = 80;
            document.getElementById('deb-slider').value = 20;
            document.getElementById('alt-min-slider').value = 35000;
            document.getElementById('alt-max-slider').value = 36000;
            document.getElementById('thresh-slider').value = 500;
            document.getElementById('ecc-slider').value = 0;
            break;
        case 'leo':
            document.getElementById('sat-slider').value = 100;
            document.getElementById('deb-slider').value = 150;
            document.getElementById('alt-min-slider').value = 400;
            document.getElementById('alt-max-slider').value = 1200;
            document.getElementById('thresh-slider').value = 800;
            break;
    }

    // Trigger change events to update values
    document.getElementById('sat-slider').dispatchEvent(new Event('input'));
    document.getElementById('deb-slider').dispatchEvent(new Event('input'));
    document.getElementById('alt-min-slider').dispatchEvent(new Event('input'));
    document.getElementById('alt-max-slider').dispatchEvent(new Event('input'));
    document.getElementById('thresh-slider').dispatchEvent(new Event('input'));
    if (document.getElementById('ecc-slider').value !== undefined) {
        document.getElementById('ecc-slider').dispatchEvent(new Event('input'));
    }

    createObjects();
    document.getElementById('status').textContent = 'READY';
}

// ============================================================================
// START APPLICATION
// ============================================================================

// Init will be called by the welcome screen after transition
// window.addEventListener('DOMContentLoaded', init);
