from cardia import Behavior, Input, Key, Serializable, on_key_pressed


class Moving(Behavior):
    def __init__(self):
        super().__init__()
        self.velocity: Serializable(int) = 0

    def on_create(self):
        self.velocity = 5

    def on_update(self, dt: float):
        ...

    @on_key_pressed(Key.Space)
    def jump(self):
        print(self.transform)
        self.transform.position.y += self.velocity * 0.0008
