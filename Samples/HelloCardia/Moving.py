from cardia import Behavior, Key, on_key_pressed, Time, Vector3, on_mouse_clicked, Mouse, Transform, PointLight
import colorsys
from Greeting import Greeting


class Moving(Behavior):
    velocity: int
    _tick_count: float
    text: str
    color: Vector3
    greeting: Greeting

    def on_create(self):
        # self.velocity = 5
        self.color = self.get_component(PointLight).color
        self._tick_count = float(colorsys.rgb_to_hsv(self.color.x, self.color.y, self.color.z)[0])

    def on_update(self):
        self._tick_count += 1
        self._tick_count %= 255.0
        h = self._tick_count / 255

        (r, g, b) = colorsys.hsv_to_rgb(h, 1, 1)
        self.color.x = r
        self.color.y = g
        self.color.z = b

        self.transform.rotation.y += self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Left)
    def move_left(self):
        self.transform.position.x -= self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Right)
    def move_right(self):
        self.transform.position.x += self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Down)
    def move_down(self):
        self.transform.position.z -= self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Up)
    def move_top(self):
        self.transform.position.z += self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Space)
    def greet(self):
        self.greeting.hello_world()
