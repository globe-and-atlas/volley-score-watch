"""Clean Pebble build and copy the verified PBW into dist/."""

from pathlib import Path
import shutil
import subprocess


ROOT = Path(__file__).resolve().parents[1]
WATCHAPP = ROOT / "watchapp"
DIST = ROOT / "dist"


def run(command: list[str], cwd: Path) -> None:
    subprocess.run(command, cwd=cwd, check=True)


def main() -> None:
    run(["pebble", "clean"], WATCHAPP)
    run(["pebble", "build"], WATCHAPP)
    source = WATCHAPP / "build" / "watchapp.pbw"
    if not source.is_file():
        raise FileNotFoundError(source)
    DIST.mkdir(parents=True, exist_ok=True)
    target = DIST / "volley-score-watch.pbw"
    shutil.copy2(source, target)
    print(target)


if __name__ == "__main__":
    main()
