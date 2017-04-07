## Takes input from a text file and plots all points.
## Connects the points in order read and connects last point to first one.
import argparse
import matplotlib.pyplot as plt

def getArgParser():
    ''' Returns ArgParser object to handle command-line args'''
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath",default="./input.ch",nargs="?",help="path of the input file")
    return parser

if __name__ == '__main__':
    parser = getArgParser()
    args = parser.parse_args()
    inputfile = open(args.filepath,"r")
    inputfile.readline(); # Discarding CH
    numberOfPoints = int(inputfile.readline())
    points = []
    for i in xrange(numberOfPoints):
        points.append(map(float,inputfile.readline().split()))
    ch_indices = map(int,inputfile.readline().split())
    for point in points:
        plt.plot(point[0],point[1],'bo')
    for i in xrange(len(ch_indices)-1):
        plt.plot([points[ch_indices[i]][0],points[ch_indices[i+1]][0]],[points[ch_indices[i]][1],points[ch_indices[i+1]][1]],'b-')
    plt.plot([points[ch_indices[0]][0],points[ch_indices[len(ch_indices)-1]][0]],[points[ch_indices[0]][1],points[ch_indices[len(ch_indices)-1]][1]],'b-')
    plt.show()
