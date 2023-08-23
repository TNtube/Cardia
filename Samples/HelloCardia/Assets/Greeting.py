from cardia import Behavior, log


class Greeting(Behavior):
    def __init__(self):
        super().__init__()
        self.message: str = "Cardia"

    def hello_world(self):
        self.message += "!"
