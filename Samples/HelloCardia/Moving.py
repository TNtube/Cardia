from cardia import Behavior, Input, Key, Serializable, on_key_pressed, Time


class Moving(Behavior):
    def __init__(self):
        super().__init__()
        self.velocity: Serializable(int) = 0

    def on_create(self):
        self.velocity = 5

    def on_update(self):
        pass

    @on_key_pressed(Key.Left)
    def move_left(self):
        self.transform.position.x -= self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Right)
    def move_right(self):
        self.transform.position.x += self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Down)
    def move_down(self):
        self.transform.position.y -= self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Up)
    def move_top(self):
        self.transform.position.y += self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Space)
    def foo(self):
        self.velocity = 10

    @on_key_pressed(Key.LeftCtrl)
    def bar(self):
        self.velocity = 5


