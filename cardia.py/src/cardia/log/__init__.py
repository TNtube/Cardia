import cardia_native as _cd
from typing import TypeVar

T = TypeVar("T")


def trace(*args: T, sep=' '):
    _cd.log.trace(' '.join(map(str, args)))


def warn(*args: T, sep=' '):
    _cd.log.warn(' '.join(map(str, args)))


def error(*args: T, sep=' '):
    _cd.log.error(' '.join(map(str, args)))


__all__ = [trace, warn, error]
