# Deployment Guide

## Your site is ready to deploy!

All files are production-ready and optimized for your domain launch.

## NEW FEATURES ADDED!

### 1. Particle Explosion Effects
- Beautiful particle explosions when satellites collide (< 5km separation)
- 30 particles per explosion with random colors (orange/yellow)
- Particles fade out realistically

### 2. Satellite Motion Trails
- Cyan-colored trails show satellite paths
- 100-point history for smooth trails
- Toggle ON/OFF with "TRAILS" button

### 3. Quick Scenario Presets
- **Crowded LEO**: 120 satellites, 200 debris, tight spacing
- **Debris Field**: Heavy debris simulation
- **Geosync Belt**: Geostationary orbit simulation
- **LEO Swarm**: Low Earth Orbit congestion

### 4. Danger Zone Visualization
- Red wireframe sphere shows collision threshold
- Updates dynamically with threshold slider
- Toggle ON/OFF with "DANGER" button

## Files to Upload

Upload these files to your web hosting:
- `index.html` - Main HTML file with welcome screen + new features
- `main.js` - Simulation engine with particle system
- `styles.css` - All styles including new buttons
- `README.md` - Documentation (optional)
- `SCENARIOS.md` - Scenarios documentation (optional)

## Quick Deploy Options

### ⭐ RECOMMENDED: Vercel (Fastest & Free)

#### Method 1: Vercel Dashboard (Easiest)
1. Go to https://vercel.com
2. Sign up/login (GitHub account works)
3. Click "Add New" → "Project"
4. Import your GitHub repo OR drag & drop your folder
5. Click "Deploy"
6. Done! Your site is live at `your-project.vercel.app`

#### Method 2: Vercel CLI
```bash
# Install Vercel CLI globally
npm i -g vercel

# Navigate to your project folder
cd path/to/orbital-collision-engine

# Deploy (first time)
vercel

# Answer the prompts:
# - Set up and deploy? Y
# - Which scope? (your account)
# - Link to existing project? N
# - Project name? orbital-collision-engine
# - Directory? ./
# - Override settings? N

# Your site is now live!
```

#### Getting a Custom Domain (.dev or any domain)

**Option 1: Buy domain through Vercel**
1. In Vercel dashboard, go to your project
2. Click "Domains"
3. Search for available .dev domain
4. Purchase directly (usually $12-20/year)
5. Auto-configured instantly!

**Option 2: Buy .dev domain elsewhere**
1. Go to https://get.dev (Google Registry)
2. Search and purchase your .dev domain
3. In Vercel dashboard: Project → Domains → Add
4. Enter your domain (e.g., `orbital-sim.dev`)
5. Add DNS records shown by Vercel to your domain registrar
6. Wait 5-60 minutes for DNS propagation
7. Done!

**Option 3: Use existing domain**
1. In Vercel: Project → Domains → Add
2. Enter your domain
3. Update your domain's DNS to point to Vercel
4. SSL certificate auto-installed

### Option 2: Netlify
1. Drag and drop your folder to netlify.com/drop
2. Get instant deployment with HTTPS
3. Custom domain: Same process as Vercel

### Option 3: GitHub Pages (Free)
1. Push your files to a GitHub repository
2. Go to Settings > Pages
3. Select your branch and save
4. Live at `https://yourusername.github.io/repo-name`

### Option 4: Traditional Web Hosting
1. Upload files via FTP/SFTP to your hosting
2. Place files in public_html or www directory
3. Access via your domain

## Features Included

✅ Beautiful cursive welcome screen
✅ Smooth fade transitions
✅ Animated starry background
✅ Responsive design (desktop & mobile)
✅ SEO meta tags
✅ Social media sharing tags
✅ Performance optimizations
✅ Cross-browser compatibility

## Custom Domain Setup

If using Netlify/Vercel:
1. Add your domain in dashboard
2. Update DNS records with your registrar
3. SSL certificate auto-configured

## Browser Support

- Chrome/Edge (latest)
- Firefox (latest)
- Safari (latest)
- Mobile browsers

## No Build Step Required

This is a static site - just upload and go!
No npm install, no build process needed.

## CDN Resources

The site uses these external CDNs:
- Three.js (3D graphics)
- Google Fonts (Great Vibes, Cinzel)

These are loaded from CDN for optimal performance.

## Pre-Deployment Checklist

Before deploying, verify everything works:

1. Open `index.html` in your browser locally
2. Verify welcome screen appears with cursive text
3. Click "Launch Simulation" or wait 5 seconds
4. Check that planets and satellites appear
5. Test these features:
   - ✅ START/PAUSE/RESET buttons work
   - ✅ Planet selector changes planets
   - ✅ TRAILS button toggles cyan satellite trails
   - ✅ DANGER button shows red collision sphere
   - ✅ Preset buttons (Crowded LEO, Debris Field, etc.)
   - ✅ Watch for particle explosions when satellites get close
6. All good? Time to deploy!

## Deployment Speed Comparison

| Method | Setup Time | Live URL Time | Custom Domain |
|--------|-----------|---------------|---------------|
| Vercel Dashboard | 2 min | Instant | +5 min |
| Vercel CLI | 1 min | Instant | +5 min |
| Netlify Drop | 1 min | Instant | +5 min |
| GitHub Pages | 5 min | 1-2 min | +10 min |

## Domain Name Ideas

If you need inspiration for a .dev domain:
- `orbital-sim.dev`
- `space-collision.dev`
- `satellite-tracker.dev`
- `orbit-engine.dev`
- `space-debris.dev`
- `orbital-mechanics.dev`

## After Deployment

1. Test your live site thoroughly
2. Share the URL!
3. Monitor with Vercel/Netlify analytics (free)
4. Optional: Add to your portfolio

## Troubleshooting

**Site shows blank screen:**
- Check browser console (F12)
- Verify all files uploaded correctly
- Clear browser cache

**Three.js not loading:**
- Check internet connection (CDN required)
- Verify HTML has correct CDN links

**Features not working:**
- Ensure main.js uploaded completely
- Check file permissions (755 for directories, 644 for files)

---

Ready to launch! 🚀

**Quick Start:** `vercel` in your terminal, that's it!
