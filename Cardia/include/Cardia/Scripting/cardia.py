import cardia_native as _cd


class Behavior:
    def __init__(self):
        self.id = None

    @property
    def transform(self):
        return _cd.get_native_transform(self.id)

    @transform.setter
    def transform(self, t):
        _cd.set_native_transform(self.id, t)

    def on_create(self):
        ...

    def on_update(self):
        ...
