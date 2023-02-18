import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import random
import copy



random.seed(109)


def GetColors(numcolors):
    css = copy.deepcopy(mcolors.CSS4_COLORS)
    del css['white']
    del css['snow']
    del css['whitesmoke']
    del css['seashell']
    del css['mistyrose']
    del css['linen']
    del css['antiquewhite']
    del css['oldlace']
    del css['floralwhite']
    del css['cornsilk']
    del css['lemonchiffon']
    del css['ivory']
    del css['lightyellow']
    del css['honeydew']
    del css['mintcream']
    del css['aliceblue']
    del css['ghostwhite']
    del css['azure']
    del css['lightcyan']
    del css['lavenderblush']
    css = list(css.values())
    tab = list(mcolors.TABLEAU_COLORS.values())
    colors = tab[:]
    while len(colors) < numcolors:
        colors.append(random.choice(css))
    return colors


if __name__ == "__main__":
    GetColors()
