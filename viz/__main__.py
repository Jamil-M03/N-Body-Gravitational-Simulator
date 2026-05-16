"""CLI: python -m viz <output_dir> [--title NAME] [--gif] [--no-anim]"""
import argparse
import sys
from pathlib import Path

import matplotlib.pyplot as plt

from . import (
    load_simulation,
    plot_trajectory, plot_velocity_field, plot_energy,
    animate,
)


def main(argv=None):
    p = argparse.ArgumentParser(description="Visualize an N-body simulation output.")
    p.add_argument("output_dir", help="Directory containing trajectory.csv and energy.csv")
    p.add_argument("--title", default=None, help="Title used for all plots")
    p.add_argument("--gif", action="store_true", help="Render animation as GIF instead of MP4")
    p.add_argument("--no-anim", action="store_true", help="Skip the animation (static plots only)")
    p.add_argument("--fps", type=int, default=30)
    p.add_argument("--trail", type=int, default=120, help="Animation trail length (frames)")
    args = p.parse_args(argv)

    out = Path(args.output_dir)
    if not out.is_dir():
        print(f"Not a directory: {out}", file=sys.stderr); return 1

    print(f"Loading from {out} ...")
    traj, energy = load_simulation(out)
    print(f"  {len(traj['body'].unique())} bodies, {len(traj['t'].unique())} frames")

    title = args.title or out.name

    print("  -> trajectory.png")
    plot_trajectory(traj, title=f"{title} — orbital paths",
                    save_path=out / "trajectory.png")
    plt.close("all")

    print("  -> velocity_field.png")
    plot_velocity_field(traj, title=f"{title} — velocity field",
                        save_path=out / "velocity_field.png")
    plt.close("all")

    print("  -> energy.png")
    plot_energy(energy, title=f"{title} — energy drift",
                save_path=out / "energy.png")
    plt.close("all")

    if not args.no_anim:
        ext = "gif" if args.gif else "mp4"
        anim_path = out / f"animation.{ext}"
        animate(traj, anim_path, title=title,
                fps=args.fps, trail_length=args.trail)

    print("Done.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
