from cardia import Behavior


class Greeting(Behavior):
    def __init__(self):
        super().__init__()
        self.message: str = "Cardia"

    def hello_world(self):
        print(f"Hello {self.message}")
