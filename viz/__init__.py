"""N-body simulation visualization package."""
from .data import load_simulation, bodies_dict
from .trajectory import plot_trajectory
from .velocity import plot_velocity_field
from .energy import plot_energy
from .animation import animate

__all__ = [
    "load_simulation", "bodies_dict",
    "plot_trajectory", "plot_velocity_field", "plot_energy",
    "animate",
]
