from setuptools import setup, find_packages
import pathlib

here = pathlib.Path(__file__).parent.resolve()
long_description = (here / "README.md").read_text(encoding="utf-8")

setup(
    name="cardia.py",
    version="0.0.1",
    description="Python API for Cardia Engine",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/TNtube/Cardia",
    author="TNtube",
    author_email="tntube.dev@gmail.com",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    package_dir={"": "src"},
    packages=find_packages(where="src"),
    python_requires='>=3.9.*'
)
