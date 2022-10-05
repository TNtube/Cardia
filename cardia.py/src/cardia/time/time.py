import cardia_native as _cd


class DeltaTime:
    def __init__(self):
        self.dt = 0.0
        self.mldt = 0.0

    def seconds(self) -> float:
        self.dt = _cd.get_delta_time_seconds()
        return self.dt

    def milliseconds(self) -> float:
        self.mldt = _cd.get_delta_time_milliseconds()
        return self.mldt

    def __float__(self):
        return self.dt


class Time:
    delta_time = DeltaTime()
