"""Velocity field: trajectories overlaid with velocity vectors as arrows.

Arrows are scaled PER BODY so that each body's mean-velocity arrow is
~15% of its own orbital radius. This keeps inner planets and outer
planets visually comparable, while still preserving relative speed
variations within a single body's orbit (useful for elliptical paths).
"""
import numpy as np
import matplotlib.pyplot as plt
from .data import bodies_dict
from .style import COLORS, apply_style


def plot_velocity_field(traj_df, title=None, n_arrows_per_body=50,
                        arrow_fraction=0.15, save_path=None, figsize=(9, 9)):
    apply_style()
    bodies = bodies_dict(traj_df)

    fig, ax = plt.subplots(figsize=figsize)

    for i, (name, df) in enumerate(bodies.items()):
        color = COLORS[i % len(COLORS)]

        # Background trajectory
        ax.plot(df["x"], df["y"], color=color, lw=0.6, alpha=0.4)

        # Characteristic radius and mean speed for this body
        r_typical = np.sqrt(df["x"]**2 + df["y"]**2).mean()
        v_mean    = np.sqrt(df["vx"]**2 + df["vy"]**2).mean()
        if v_mean < 1e-12 or r_typical < 1e-12:
            continue  # near-stationary body (e.g. the Sun); skip arrows

        # Target arrow length (in plot data units) = fraction of r_typical
        # Scale factor s such that v_mean * s = arrow_fraction * r_typical
        s = (arrow_fraction * r_typical) / v_mean

        # Subsample evenly along the trajectory
        step = max(1, len(df) // n_arrows_per_body)
        sub = df.iloc[::step]

        ax.quiver(sub["x"], sub["y"], sub["vx"] * s, sub["vy"] * s,
                  color=color, alpha=0.9,
                  angles="xy", scale_units="xy", scale=1.0,
                  width=0.003, headwidth=4, label=name)

    ax.set_xlabel("x (AU)")
    ax.set_ylabel("y (AU)")
    ax.set_aspect("equal")
    if title:
        ax.set_title(title)
    ax.legend(loc="best")

    if save_path:
        fig.savefig(save_path, dpi=150, bbox_inches="tight")
    return fig, ax
