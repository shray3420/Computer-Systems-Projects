# 
# This file was created as part of CS 340's Spring 2022 Final Project
# - This file was further modified for MP9.
# - It should not need to be modified further for you to complete MP9 -- you need only focus on the MGs! :)
#

class Coord:
    def __init__(self, row, col):
        self.row = row
        self.col = col

    def __add__(self, coord):
        return Coord(self.row + coord.row, self.col + coord.col)

    def __sub__(self, coord):
        return Coord(self.row - coord.row, self.col - coord.col)

    def __eq__(self, coord):
        return self.row == coord.row and self.col == coord.col
