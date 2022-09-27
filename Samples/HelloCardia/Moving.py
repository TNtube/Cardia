from cardia import Behavior, Input, Key, Serializable, on_key_pressed


class Moving(Behavior):
    def __init__(self):
        super().__init__()
        self.velocity: Serializable(int) = 0

    def on_create(self):
        self.velocity = 5

    def on_update(self, dt: float):
        if Input.is_key_pressed(Key.Space):
            print("Space is called !")

    @on_key_pressed(Key.Left)
    def move_left(self):
        self.transform.position.x -= self.velocity * 0.0008


@on_key_pressed(Key.Up)
def foo():
    print("bar")

