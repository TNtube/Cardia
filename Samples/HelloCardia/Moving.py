from cardia import Behavior, Input, Key, Serializable, on_key_pressed,\
    Time, Vector3, on_mouse_clicked, Mouse


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

    @on_mouse_clicked(Mouse.Left)
    def clicked(self):
        v = Input.get_mouse_position()
        print(f"Mouse pos is ({v.x}:{v.y})")

    @on_mouse_clicked(Mouse.Right)
    def teleport(self):
        self.transform.position = Vector3(7, 4, 0)
