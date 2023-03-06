from cardia import Behavior


class Greeting(Behavior):
    message: str

    def hello_world(self):
        print(f"Hello {self.message}")
