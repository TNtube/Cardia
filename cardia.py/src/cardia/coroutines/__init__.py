import types
from .event_loop import CustomEventLoop


async def wait_for_frames(frames: int = 1):
    for _ in range(frames):
        await _wait()


@types.coroutine
def _wait():
    yield
