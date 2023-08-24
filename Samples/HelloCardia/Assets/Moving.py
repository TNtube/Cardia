from cardia import Behavior, Time, Vector3, Input, Key


class Moving(Behavior):
    def __init__(self):
        super().__init__()
        self.velocity: int = 300
        self.remaining_angle: float = 0.0

        self.routine = None
        self.is_rotating = False

    def on_create(self):
        # self.velocity = 5
        pass

    def on_update(self):
        if self.routine is not None:
            try:
                next(self.routine)
            except StopIteration:
                self.routine = None

        if self.is_rotating:
            return

        if Input.is_key_pressed(Key.Left):
            self.routine = self.roll(Vector3.left)
        elif Input.is_key_pressed(Key.Right):
            self.routine = self.roll(Vector3.right)
        elif Input.is_key_pressed(Key.Up):
            self.routine = self.roll(Vector3.forward)
        elif Input.is_key_pressed(Key.Down):
            self.routine = self.roll(Vector3.back)

    def roll(self, direction: Vector3) -> iter:
        self.is_rotating = True
        self.remaining_angle = 90
        center = self.transform.position + direction + Vector3.down
        axis = Vector3.up.cross(direction)
        while self.remaining_angle > 0:
            angle = min(self.remaining_angle, self.velocity * Time.delta_time.seconds())
            self.transform.rotate_around(center, axis, angle)
            self.remaining_angle -= angle
            yield

        self.is_rotating = False
