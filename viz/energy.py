"""Energy drift over time — diagnostic for integrator quality."""
import numpy as np
import matplotlib.pyplot as plt
from .style import apply_style


def plot_energy(energy_df, title=None, save_path=None, figsize=(10, 5)):
    """Plot |ΔE / E₀| versus time on a log scale.

    A working RK4 integrator should keep this near machine epsilon
    (~1e-14 to 1e-9 depending on dt and system stiffness) with only
    very slow secular growth. Exponential blow-up indicates a bug
    or a too-large timestep.
    """
    apply_style()

    fig, ax = plt.subplots(figsize=figsize)

    t = energy_df["t"]
    # Take absolute value; replace zeros with a floor so log scale doesn't barf
    drift = np.abs(energy_df["relative_drift"].values)
    floor = 1e-18
    drift = np.where(drift > floor, drift, floor)

    ax.semilogy(t, drift, color="#ff6b6b", lw=1.5)
    ax.set_xlabel("time (yr)")
    ax.set_ylabel(r"$|\Delta E / E_0|$")
    if title:
        ax.set_title(title)
    ax.set_ylim(bottom=floor)

    # Annotate the maximum drift in the corner — handy diagnostic
    max_drift = drift.max()
    ax.text(0.98, 0.05, f"max drift: {max_drift:.2e}",
            transform=ax.transAxes, ha="right", va="bottom",
            fontsize=10, color="#ff6b6b",
            bbox=dict(facecolor="#0a0e1a", edgecolor="#ff6b6b", alpha=0.7))

    if save_path:
        fig.savefig(save_path, dpi=150, bbox_inches="tight")
    return fig, ax
