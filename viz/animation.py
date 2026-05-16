"""Animated orbits with fading trails — the centerpiece visualization."""
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as mpl_animation
from matplotlib.collections import LineCollection

from .data import bodies_dict
from .style import COLORS, apply_style


def animate(traj_df, save_path, title=None,
            fps=30, trail_length=120, figsize=(10, 10), dpi=100,
            verbose=True):
    """Render an animated MP4 or GIF of the simulation.

    Parameters
    ----------
    traj_df : DataFrame   trajectory CSV loaded into pandas
    save_path : str/Path  output filename; extension chooses format
                          ('.mp4' uses ffmpeg, '.gif' uses Pillow)
    fps : int             frames per second of output animation
    trail_length : int    how many past frames to render as fading trail
    """
    apply_style()
    bodies = bodies_dict(traj_df)
    body_names = list(bodies.keys())
    times = sorted(traj_df["t"].unique())
    n_frames = len(times)

    # Stack coords for fast frame-indexed access: arr[body_idx, frame, axis]
    xs = np.stack([bodies[n]["x"].values for n in body_names])
    ys = np.stack([bodies[n]["y"].values for n in body_names])

    # Compute fixed plot bounds with padding (avoid the view jittering)
    pad = 0.08 * max(np.ptp(xs), np.ptp(ys))
    xlim = (xs.min() - pad, xs.max() + pad)
    ylim = (ys.min() - pad, ys.max() + pad)

    fig, ax = plt.subplots(figsize=figsize)
    ax.set_xlim(xlim); ax.set_ylim(ylim)
    ax.set_aspect("equal")
    ax.set_xlabel("x (AU)")
    ax.set_ylabel("y (AU)")
    if title:
        ax.set_title(title)

    # One LineCollection per body for the fading trail.
    # LineCollection takes a sequence of line segments and an array of
    # alphas/colors — perfect for "last N steps fade from solid to faded".
    trail_collections = []
    body_dots = []
    for i, name in enumerate(body_names):
        color = COLORS[i % len(COLORS)]
        lc = LineCollection([], colors=color, linewidths=1.5)
        ax.add_collection(lc)
        trail_collections.append(lc)

        # The "current position" marker for this body
        (dot,) = ax.plot([xs[i, 0]], [ys[i, 0]], "o",
                         color=color, markersize=9,
                         markeredgecolor="white", markeredgewidth=0.6,
                         label=name)
        body_dots.append(dot)

    ax.legend(loc="upper right")
    time_text = ax.text(0.02, 0.97, "", transform=ax.transAxes,
                        fontsize=11, va="top",
                        bbox=dict(facecolor="#0a0e1a", edgecolor="#3a3f50",
                                  alpha=0.7, boxstyle="round,pad=0.3"))

    # Precompute an alpha ramp used for fading every body's trail
    alphas = np.linspace(0.05, 0.9, trail_length)

    def update(frame):
        artists = []
        for i in range(len(body_names)):
            # Update the current-position dot
            body_dots[i].set_data([xs[i, frame]], [ys[i, frame]])
            artists.append(body_dots[i])

            # Build fading trail: segments from (frame - trail_length) to frame
            start = max(0, frame - trail_length)
            xt = xs[i, start:frame + 1]
            yt = ys[i, start:frame + 1]

            if len(xt) >= 2:
                # Build N-1 line segments connecting consecutive points
                segs = np.stack([
                    np.column_stack([xt[:-1], yt[:-1]]),
                    np.column_stack([xt[1:],  yt[1:]]),
                ], axis=1)
                trail_collections[i].set_segments(segs)
                # Fade alpha across the trail length
                n_segs = len(segs)
                trail_alphas = alphas[-n_segs:]
                color = COLORS[i % len(COLORS)]
                # LineCollection takes RGBA per segment
                rgba = np.tile(mpl_to_rgba(color), (n_segs, 1))
                rgba[:, 3] = trail_alphas
                trail_collections[i].set_colors(rgba)
            else:
                trail_collections[i].set_segments([])
            artists.append(trail_collections[i])

        time_text.set_text(f"t = {times[frame]:.3f} yr")
        artists.append(time_text)
        return artists

    if verbose:
        print(f"Rendering {n_frames} frames at {fps} fps -> {save_path}")

    anim = mpl_animation.FuncAnimation(
        fig, update, frames=n_frames,
        interval=1000.0 / fps, blit=True,
    )

    save_path = str(save_path)
    if save_path.endswith(".mp4"):
        writer = mpl_animation.FFMpegWriter(fps=fps, bitrate=2500,
                                            codec="libx264",
                                            extra_args=["-pix_fmt", "yuv420p"])
    elif save_path.endswith(".gif"):
        writer = mpl_animation.PillowWriter(fps=fps)
    else:
        raise ValueError(f"Unrecognised output extension: {save_path!r}. "
                         "Use .mp4 or .gif.")

    anim.save(save_path, writer=writer, dpi=dpi)
    plt.close(fig)


def mpl_to_rgba(c):
    """Convert a hex color string to an [R, G, B, A] array (A defaulting to 1)."""
    from matplotlib.colors import to_rgba
    return np.array(to_rgba(c))
