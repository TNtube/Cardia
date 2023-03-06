from cardia import Behavior, Key, on_key_pressed, Time, Vector3, on_mouse_clicked, Mouse, Transform, PointLight, Vector4
import colorsys
from Greeting import Greeting


class Moving(Behavior):
    velocity: int
    _tick_count: float
    text: str
    color: Vector3
    greeting: Greeting
    lst: list[str]

    def on_create(self):
        # self.velocity = 5
        pass

    def on_update(self):
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
        # self.greeting.hello_world()
        print("message from python")
        print("|".join(map(str, self.lst)))
