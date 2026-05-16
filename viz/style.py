"""Shared styling for all viz outputs."""
import matplotlib as mpl

# Distinct, vivid colors that read well on a dark background.
# Order matters: assigned to bodies in the order they appear in the CSV.
COLORS = [
    "#ffcc00",  # solar yellow
    "#ff6b6b",  # coral
    "#4ecdc4",  # teal
    "#a78bfa",  # violet
    "#fb923c",  # orange
    "#34d399",  # mint
    "#f472b6",  # pink
    "#60a5fa",  # sky
]

# Background colors
BG_DARK    = "#0a0e1a"
FG_LIGHT   = "#e6e8ee"
GRID_COLOR = "#1f2434"


def apply_style():
    """Apply our dark theme globally. Idempotent — safe to call repeatedly."""
    mpl.rcParams.update({
        "figure.facecolor":  BG_DARK,
        "axes.facecolor":    BG_DARK,
        "savefig.facecolor": BG_DARK,
        "axes.edgecolor":    FG_LIGHT,
        "axes.labelcolor":   FG_LIGHT,
        "xtick.color":       FG_LIGHT,
        "ytick.color":       FG_LIGHT,
        "text.color":        FG_LIGHT,
        "grid.color":        GRID_COLOR,
        "grid.alpha":        0.6,
        "axes.grid":         True,
        "axes.spines.top":   False,
        "axes.spines.right": False,
        "legend.framealpha": 0.0,
        "legend.labelcolor": FG_LIGHT,
        "font.family":       "DejaVu Sans",
        "font.size":         11,
        "axes.titlesize":    14,
        "axes.titleweight":  "bold",
    })
