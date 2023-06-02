# 
# This file was created as part of CS 340's Spring 2022 Final Project
# - This file was further modified for MP9.
# - It should not need to be modified further for you to complete MP9 -- you need only focus on the MGs! :)
#

from maze.maze import Maze


class MazeGenerator:
    def __init__(self, num_rows=7, num_cols=7):
        self.height = num_rows
        self.width = num_cols
        self.maze = Maze(num_rows, num_cols)

    def create(self) -> Maze:
        return self.maze
