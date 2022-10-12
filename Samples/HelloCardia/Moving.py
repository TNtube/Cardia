from cardia import Behavior, Key, on_key_pressed, Time, Vector3, on_mouse_clicked, Mouse, Transform


class Moving(Behavior):
    velocity: int
    _mouse_count: int

    def on_create(self):
        self.velocity = 5
        self._mouse_count = 0

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
        self._mouse_count += 1
        self.get_component(Transform).reset()

    @on_mouse_clicked(Mouse.Right)
    def teleport(self):
        self._mouse_count += 1
        self.transform.position = Vector3.lerp(self.transform.position, Vector3(7, 4, 0),
                                               5 * Time.delta_time.seconds())
