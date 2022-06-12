from cardia import *


class Moving(EntityBehavior):
    def start(self):
        self.velocity = 0.05

    def update(self):
        if Input.is_key_pressed(key.A):
            self.transform.position.x -= self.velocity
        if Input.is_key_pressed(key.D):
            self.transform.position.x += self.velocity
        if Input.is_key_pressed(key.S):
            self.transform.position.y -= self.velocity
        if Input.is_key_pressed(key.W):
            self.transform.position.y += self.velocity

        if Input.is_key_pressed(key.space):
            self.transform.rotation.x += self.velocity
