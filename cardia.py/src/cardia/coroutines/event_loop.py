import asyncio
import time
from asyncio import events

from .. import log


class CustomEventLoop(asyncio.AbstractEventLoop):
    def __init__(self, start_loop=False):
        self._scheduled = []
        self._running = False
        self._debug = False
        self._exc = None
        if start_loop:
            events._set_running_loop(self)

    def is_running(self):
        return self._running

    def stop(self):
        self._running = False

    def create_task(self, coro, *, name=None, context=None):
        task = asyncio.Task(coro, loop=self, name=name, context=context)
        return task

    def create_future(self):
        return asyncio.Future(loop=self)

    def run_forever(self):
        events._set_running_loop(self)
        self._running = True
        while self._running:
            self.run_all_once()
        self._running = False

    def run_until_complete(self, future):
        events._set_running_loop(self)
        self._running = True
        future = asyncio.ensure_future(future, loop=self)
        future.add_done_callback(lambda _: self.stop())
        self.run_forever()
        return future.result()

    def call_soon(self, callback, *args, context=None):
        h = asyncio.TimerHandle(self.time(), callback, args, self, context)
        self._scheduled.append(h)
        return h

    def call_later(self, delay, callback, *args, context=None):
        if delay < 0:
            raise Exception("Can't schedule in the past")
        h = asyncio.TimerHandle(self.time() + delay, callback, args, self, context)
        self._scheduled.append(h)
        return h

    def call_at(self, when, callback, *args, context=None):
        if when < self.time():
            raise Exception("Can't schedule in the past")
        h = asyncio.TimerHandle(when, callback, args, self, context)
        self._scheduled.append(h)
        return h

    def time(self):
        return time.monotonic()

    def _timer_handle_cancelled(self, handle):
        pass

    def run_all_once(self):
        if not self._scheduled:
            return
        self._running = True
        to_run = self._scheduled[:]
        self._scheduled.clear()
        for handle in to_run:
            if handle.cancelled():
                continue
            if handle.when() <= self.time():
                handle._run()
            else:
                self._scheduled.append(handle)

            if self._exc is not None:
                log.error(self._exc)
        self._exc = None
        self._running = False

    def get_debug(self):
        return self._debug

    def set_debug(self, enabled):
        self._debug = enabled

    def call_exception_handler(self, context):
        self._exc = context.get('exception', None)


