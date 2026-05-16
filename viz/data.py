"""Load simulation output CSVs into pandas DataFrames."""
from pathlib import Path
import pandas as pd


def load_simulation(output_dir):
    """Read trajectory.csv and energy.csv from a simulation output directory.

    Returns
    -------
    traj : pd.DataFrame   with columns t, body, x, y, z, vx, vy, vz
    energy : pd.DataFrame with columns t, energy, relative_drift
    """
    output_dir = Path(output_dir)
    traj = pd.read_csv(output_dir / "trajectory.csv")
    energy = pd.read_csv(output_dir / "energy.csv")
    return traj, energy


def bodies_dict(traj_df):
    """Split the long-format trajectory frame into one frame per body.

    Returns an ordered dict: {body_name: sub_df} in the order bodies
    first appear in the CSV (which matches their order in the JSON config).
    Each sub_df is indexed 0..N-1 in time order.
    """
    seen = []
    groups = {}
    for name, g in traj_df.groupby("body", sort=False):
        if name not in groups:
            seen.append(name)
            groups[name] = g.reset_index(drop=True)
    return {name: groups[name] for name in seen}
