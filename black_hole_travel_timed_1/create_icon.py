"""
Black Hole Icon Generator
Creates a simple black hole icon for the application
Requires: pip install pillow
"""

from PIL import Image, ImageDraw

def create_black_hole_icon():
    """Create a simple black hole icon with accretion disk"""

    # Create image with transparency (256x256 for high quality)
    size = 256
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    center = size // 2

    # Draw accretion disk glow (outer glow)
    for i in range(10, 0, -1):
        radius = center - 10 + i * 8
        alpha = int(255 * (i / 10) * 0.3)
        color = (255, 140, 0, alpha)  # Orange glow
        draw.ellipse([center - radius, center - radius,
                     center + radius, center + radius],
                     fill=color)

    # Draw bright accretion disk
    disk_radius = center - 30
    for i in range(15, 0, -1):
        r = disk_radius - i * 2
        alpha = int(255 * (i / 15))
        # Gradient from orange to yellow
        color = (255, 140 + int(115 * i / 15), 0, alpha)
        draw.ellipse([center - r, center - r, center + r, center + r],
                     fill=color)

    # Draw event horizon (black circle)
    horizon_radius = center - 60
    draw.ellipse([center - horizon_radius, center - horizon_radius,
                 center + horizon_radius, center + horizon_radius],
                 fill=(0, 0, 0, 255))

    # Draw gravitational lensing effect (rings around black hole)
    for ring in range(3):
        ring_radius = horizon_radius + 5 + ring * 8
        ring_thickness = 2
        # Draw outer circle
        draw.ellipse([center - ring_radius, center - ring_radius,
                     center + ring_radius, center + ring_radius],
                     outline=(100, 100, 150, 150), width=ring_thickness)

    # Save as ICO file with multiple sizes
    icon_sizes = [(256, 256), (128, 128), (64, 64), (48, 48), (32, 32), (16, 16)]

    # Create resized versions
    images = []
    for icon_size in icon_sizes:
        resized = img.resize(icon_size, Image.Resampling.LANCZOS)
        images.append(resized)

    # Save as .ico file
    images[0].save('black_hole_icon.ico', format='ICO',
                   sizes=[(img.width, img.height) for img in images],
                   append_images=images[1:])

    print("[OK] Icon created successfully: black_hole_icon.ico")
    print("     Multiple sizes included: 256x256, 128x128, 64x64, 48x48, 32x32, 16x16")

    # Also save as PNG for preview
    img.save('black_hole_icon_preview.png')
    print("[OK] Preview saved: black_hole_icon_preview.png")

if __name__ == "__main__":
    print("Creating Black Hole icon...")
    create_black_hole_icon()
    print("\nDone! The icon is ready to use in your application.")
