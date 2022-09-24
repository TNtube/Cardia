from cardia import Behavior, Input, Key


class Moving(Behavior):
    def __init__(self):
        super().__init__()
        self.velocity = 0

    def on_create(self):
        self.velocity = 0.05

    def on_update(self):
        if Input.is_key_pressed(Key.Space):
            t = self.transform
            t.position.x += self.velocity
            self.transform = t

        # if Input.is_key_pressed(key.A):
        #     self.transform.scale.x -= self.velocity
        # if Input.is_key_pressed(key.D):
        #     self.transform.scale.x += self.velocity
        # if Input.is_key_pressed(key.S):
        #     self.transform.position.y -= self.velocity
        # if Input.is_key_pressed(key.W):
        #     self.transform.position.y += self.velocity
