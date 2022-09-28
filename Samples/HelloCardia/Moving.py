from cardia import Behavior, Input, Key, Serializable, on_key_pressed, Time


class Moving(Behavior):
    def __init__(self):
        super().__init__()
        self.velocity: Serializable(int) = 0

    def on_create(self):
        self.velocity = 5

    def on_update(self):
        if Input.is_key_pressed(Key.Space):
            print(Time.delta_time.seconds())

    @on_key_pressed(Key.Left)
    def move_left(self):
        self.transform.position.x -= self.velocity * 0.0008


@on_key_pressed(Key.Up)
def foo():
    print("bar")

