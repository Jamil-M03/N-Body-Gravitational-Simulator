"""Static plot of orbital paths."""
import matplotlib.pyplot as plt
from .data import bodies_dict
from .style import COLORS, apply_style


def plot_trajectory(traj_df, title=None, ax=None, save_path=None, figsize=(9, 9)):
    """Plot 2D orbital paths in the X-Y plane.

    Each body is rendered as a continuous line in its assigned color,
    with a square at the starting position and a star at the final
    position. The axes share an equal aspect ratio so circular orbits
    look circular.
    """
    apply_style()
    bodies = bodies_dict(traj_df)

    if ax is None:
        fig, ax = plt.subplots(figsize=figsize)
    else:
        fig = ax.figure

    for i, (name, df) in enumerate(bodies.items()):
        color = COLORS[i % len(COLORS)]
        ax.plot(df["x"], df["y"], color=color, lw=1.4, alpha=0.85, label=name)
        # Start marker (square)
        ax.scatter(df["x"].iloc[0], df["y"].iloc[0],
                   marker="s", s=40, color=color, zorder=5,
                   edgecolor="white", linewidth=0.5)
        # End marker (star)
        ax.scatter(df["x"].iloc[-1], df["y"].iloc[-1],
                   marker="*", s=180, color=color, zorder=6,
                   edgecolor="white", linewidth=0.5)

    ax.set_xlabel("x (AU)")
    ax.set_ylabel("y (AU)")
    ax.set_aspect("equal")
    if title:
        ax.set_title(title)
    ax.legend(loc="best")

    if save_path:
        fig.savefig(save_path, dpi=150, bbox_inches="tight")
    return fig, ax
