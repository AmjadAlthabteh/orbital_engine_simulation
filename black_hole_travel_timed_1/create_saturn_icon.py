"""
Saturn Icon Generator - Alternative space icon
Creates a Saturn planet with rings icon
"""

from PIL import Image, ImageDraw

def create_saturn_icon():
    """Create a Saturn icon with rings"""

    size = 256
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    center = size // 2

    # Draw space glow background
    for i in range(8, 0, -1):
        radius = center - 10 + i * 10
        alpha = int(255 * (i / 8) * 0.15)
        color = (100, 100, 200, alpha)
        draw.ellipse([center - radius, center - radius,
                     center + radius, center + radius],
                     fill=color)

    # Draw Saturn's rings (behind planet)
    ring_color = (200, 180, 140, 255)
    ring_shadow = (80, 70, 50, 255)

    # Outer ring
    ring_outer = 100
    ring_inner = 65
    ring_height = 30

    # Draw ring shadow (darker, behind)
    draw.ellipse([center - ring_outer, center - ring_height,
                 center + ring_outer, center + ring_height],
                 fill=ring_shadow)
    draw.ellipse([center - ring_inner, center - ring_height + 8,
                 center + ring_inner, center + ring_height - 8],
                 fill=(0, 0, 0, 0))

    # Draw Saturn planet
    planet_radius = 60

    # Planet gradient (golden/tan color)
    for i in range(planet_radius, 0, -2):
        progress = i / planet_radius
        r = int(220 - 20 * (1 - progress))
        g = int(180 - 30 * (1 - progress))
        b = int(100 - 20 * (1 - progress))
        draw.ellipse([center - i, center - i, center + i, center + i],
                     fill=(r, g, b, 255))

    # Draw atmospheric bands (horizontal stripes)
    for y in range(-planet_radius, planet_radius, 15):
        band_alpha = 40
        band_color = (150, 130, 80, band_alpha)
        y_pos = center + y
        x_width = int(planet_radius * (1 - (y / planet_radius) ** 2) ** 0.5)
        draw.ellipse([center - x_width, y_pos - 3,
                     center + x_width, y_pos + 3],
                     fill=band_color)

    # Draw ring (in front of planet)
    draw.ellipse([center - ring_outer, center - ring_height,
                 center + ring_outer, center + ring_height],
                 fill=ring_color)
    draw.ellipse([center - ring_inner, center - ring_height + 8,
                 center + ring_inner, center + ring_height - 8],
                 fill=(0, 0, 0, 0))

    # Add ring divisions (Cassini Division)
    division_outer = 85
    division_inner = 80
    draw.ellipse([center - division_outer, center - ring_height + 3,
                 center + division_outer, center + ring_height - 3],
                 outline=(100, 90, 60, 200), width=2)

    # Highlight on planet (sun reflection)
    highlight_pos = (center - 25, center - 25)
    for i in range(20, 0, -2):
        alpha = int(100 * (i / 20))
        draw.ellipse([highlight_pos[0] - i, highlight_pos[1] - i,
                     highlight_pos[0] + i, highlight_pos[1] + i],
                     fill=(255, 255, 200, alpha))

    # Save multiple sizes
    icon_sizes = [(256, 256), (128, 128), (64, 64), (48, 48), (32, 32), (16, 16)]
    images = []
    for icon_size in icon_sizes:
        resized = img.resize(icon_size, Image.Resampling.LANCZOS)
        images.append(resized)

    images[0].save('saturn_icon.ico', format='ICO',
                   sizes=[(img.width, img.height) for img in images],
                   append_images=images[1:])

    print("[OK] Saturn icon created: saturn_icon.ico")

    img.save('saturn_icon_preview.png')
    print("[OK] Preview saved: saturn_icon_preview.png")

if __name__ == "__main__":
    print("Creating Saturn icon...")
    create_saturn_icon()
    print("\nDone!")
