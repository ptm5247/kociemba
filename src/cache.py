from array import array
from math import comb, factorial
from pathlib import Path

# Corners
N_CORNERS = 8
URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB = range(N_CORNERS)

# Edges
N_EDGES = 12
UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR = range(N_EDGES)

# moves
N_MOVES = 6
U, R, F, D, L, B = range(N_MOVES)
M = 6 # follows L
S = 7 # follows F

# coordinate spaces
N_TWIST    = 2187
N_FLIP     = 2048
N_SLICE   = 11880
N_CORNER   = 20160
N_EDGEUD  = 20160
N_EDGEU   = 1320
N_EDGED   = 1320

N_SLICE1   = 495
N_SLICE2   = 24
N_PARITY   = 2

MN_EDGEU   = 336
MN_EDGED   = 336

class Cube:

  # Permutation Table: int[N_MOVES][N_PIECES]
  # For a move M and an index I, the piece at position I is replaced by the
  # piece at position PRM_TABLE[M][I].
  PRM_TABLE: list[list[int]]
  # Orientation Table: int[N_MOVES][N_PIECES]
  # For a move M and an index I, the orientation of the piece replacing the one
  # at position I is increased by ORI_TABLE[M][I].
  ORI_TABLE: list[list[int]]
  # The number of pieces tracked by this type of cube
  N_PIECES: int             # 8 for EdgeCube, 12 for CornerCube
  # The number of possible orientation for this cube's type of piece
  FACELETS_PER_PIECE: int   # 2 for EdgeCube,  3 for CornerCube

  def __init__(self) -> None:
    self.set_solved()

  def set_solved(self) -> None:
    self.prm = list(range(self.N_PIECES))
    self.ori = [ 0 ] * self.N_PIECES

  def __imatmul__(self, move: int):
    prm_dest: list[int] = []
    ori_dest: list[int] = []
    for p, o in zip(self.PRM_TABLE[move], self.ORI_TABLE[move]):
      prm_dest.append(self.prm[p])
      ori_dest.append((self.ori[p] + o) % self.FACELETS_PER_PIECE)
    self.prm = prm_dest
    self.ori = ori_dest
    return self

  # ori - orientation coordinate
  #
  # The orientation of a piece is the number of CCW turns (or flips) required to
  # get a piece's primary color onto the piece's primary face. The primary color
  # is the U or D color (or the F or B color for edges that contain nether the U
  # color nor the D color), and the primary face is the U or D face (or the F or
  # B face for edges which touch neither the U face nor the D face).
  #
  # The orientation coordinate is an X-digit, base-Y number (with Y^X values)
  # where X=N_PIECES-1 and Y=FACELETS_PER_PIECE. The Nth digit (from the left)
  # is the orientation of the piece in position N (i.e. for n=0 on a CornerCube,
  # the corner in the URF position, not the URF corner itself). The sum of all
  # orientations must be divisible by FACELETS_PER_PIECE, so the orientation of
  # the final piece can be determined by parity.

  def get_ori(self) -> int:
    ori = 0
    for o in self.ori[:-1]:
      ori = (ori * self.FACELETS_PER_PIECE) + o
    return ori

  def set_ori(self, ori: int) -> None:
    parity = 0
    for i in reversed(range(self.N_PIECES - 1)):
      ori, self.ori[i] = divmod(ori, self.FACELETS_PER_PIECE)
      parity += self.ori[i]
    # set orientation of last piece by parity
    self.ori[-1] = (self.FACELETS_PER_PIECE - parity) % self.FACELETS_PER_PIECE

  # perm - permutation coordinate
  #
  # The permutation has two components: combination and order.
  # The value is determined by: perm = (combination * MAX_ORDER) + order.
  # The permutation is defined for a subset S of this cube's piece list. For
  # example, slice coordinate is the permutation of the E-slice edges. So, for
  # the slice coordinate, S = { FR, FL, BL, BR }.
  #
  # For the slice coordinate example, consider the following table:
  #   Postion: | UR | UF | UL | UB | DR | DF | DL | DB | FR | FL | BL | BR |
  #         n: |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 |
  # Occupied?: |    | FR |    |    | BL |    |    |    | FL | BR |    |    |
  # where the Occupied? row indicates the positions of each member of S.
  #
  # The combination component has (N_PIECES choose len(S)) possible values.
  # Consider a bitmask constructed from the Occupied? row of the above table.
  # The combination component = the sum of nCk for all set bits, where n = the
  # bit position (from the left) and k-1 = the number of set bits to the left of
  # bit n. For the above table (bitmask = 010010001100):
  # combination = 1C1 + 4C2 + 8C3 + 9C4 = 189
  #
  # The order component has len(S)! possible values.
  # Consider the ordering of S defined by reading the Occupied? row of the table
  # from left to right. To calculate the order component:
  # For I = len(S) downto 2:
  #   Rotate the first I elements of S to the left K times such that the largest
  #   of the first I elements of S ends up in the rightmost position.
  #   Increment the order component by K * (I-1)!
  # For the above table (S_ord = [ FR, BL, FL, BR ]):
  # order = 0*3! + 2*2! + 1*1! = 5

  def get_prm(self, subset: list[int], *, right_handed=False) -> int:
    MAX_ORDER = factorial(len(subset))
    ordered = []
    combination, order = 0, 0

    if right_handed:
      for n, piece in reversed(list(enumerate(self.prm))):
        if piece in subset:
          ordered.insert(0, piece)
          combination += comb(self.N_PIECES-1 - n, len(ordered))
    else:
      for n, piece in enumerate(self.prm):
        if piece in subset:
          ordered.append(piece)
          combination += comb(n, len(ordered))

    for i in reversed(range(1, len(subset))):
      partial = (ordered.index(subset[i]) - i) % (i + 1)
      ordered[:i+1] = ordered[partial:i+1] + ordered[:partial]
      order = ((i + 1) * order) + partial

    return (combination * MAX_ORDER) + order

  def set_prm(self, prm: int, subset: list[int], *, right_handed=False) -> None:
    MAX_ORDER = factorial(len(subset))
    combination, order = divmod(prm, MAX_ORDER)
    placeholder = next(p for p in self.prm[::-1] if p not in subset)
    self.prm = [ placeholder ] * self.N_PIECES

    for i in range(1, len(subset)): # reconstruct the permutation
      order, partial = divmod(order, i + 1)
      subset[:i+1] = subset[:i+1][-partial:] + subset[:i+1][:-partial]

    if right_handed:
      n = 0
      k = len(subset) - 1
      while k >= 0: # insert pieces into their positions
        if combination >= (partial := comb(self.N_PIECES-1 - n, k + 1)):
          self.prm[n] = subset[-(k+1)]
          combination -= partial
          k -= 1
        n += 1
    else:
      n = self.N_PIECES - 1
      k = len(subset) - 1
      while k >= 0: # insert pieces into their positions
        if combination >= (partial := comb(n, k + 1)):
          self.prm[n] = subset[k]
          combination -= partial
          k -= 1
        n -= 1

################################################################################

class CornerCube(Cube):

  PRM_TABLE = [
    [ UBR, URF, UFL, ULB, DFR, DLF, DBL, DRB, ], # U
    [ DFR, UFL, ULB, URF, DRB, DLF, DBL, UBR, ], # R
    [ UFL, DLF, ULB, UBR, URF, DFR, DBL, DRB, ], # F
    [ URF, UFL, ULB, UBR, DLF, DBL, DRB, DFR, ], # D
    [ URF, ULB, DBL, UBR, DFR, UFL, DLF, DRB, ], # L
    [ URF, UFL, UBR, DRB, DFR, DLF, ULB, DBL, ], # B
    [ DFR, ULB, DBL, URF, DRB, UFL, DLF, UBR, ], # M
    [ UFL, DLF, UBR, DRB, URF, DFR, ULB, DBL, ], # S
  ]
  ORI_TABLE = [
    [   0,   0,   0,   0,   0,   0,   0,   0, ], # U
    [   2,   0,   0,   1,   1,   0,   0,   2, ], # R
    [   1,   2,   0,   0,   2,   1,   0,   0, ], # F
    [   0,   0,   0,   0,   0,   0,   0,   0, ], # D
    [   0,   1,   2,   0,   0,   2,   1,   0, ], # L
    [   0,   0,   1,   2,   0,   0,   2,   1, ], # B
    [   2,   1,   2,   1,   1,   2,   1,   2, ], # M
    [   1,   2,   1,   2,   2,   1,   2,   1, ], # S
  ]
  N_PIECES = N_CORNERS
  FACELETS_PER_PIECE = 3

  # twist: corner orientation coordinate [0, 3^7) (2187)

  def set_twist(self, twist: int) -> None:
    return self.set_ori(twist)

  def get_twist(self) -> int:
    return self.get_ori()

  # corner: corner permutation coordinate [0, 8C6 * 6!) (20160)

  def set_corner(self, corner: int) -> None:
    return self.set_prm(corner, [URF, UFL, ULB, UBR, DFR, DLF])

  def get_corner(self) -> int:
    return self.get_prm([URF, UFL, ULB, UBR, DFR, DLF])

class EdgeCube(Cube):

  PRM_TABLE = [
    [ UB, UR, UF, UL, DR, DF, DL, DB, FR, FL, BL, BR, ], # U
    [ FR, UF, UL, UB, BR, DF, DL, DB, DR, FL, BL, UR, ], # R
    [ UR, FL, UL, UB, DR, FR, DL, DB, UF, DF, BL, BR, ], # F
    [ UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR, ], # D
    [ UR, UF, BL, UB, DR, DF, FL, DB, FR, UL, DL, BR, ], # L
    [ UR, UF, UL, BR, DR, DF, DL, BL, FR, FL, UB, DB, ], # B
    [ FR, UF, BL, UB, BR, DF, FL, DB, DR, UL, DL, UR, ], # M
    [ UR, FL, UL, BR, DR, FR, DL, BL, UF, DF, UB, DB, ], # S
  ]
  ORI_TABLE = [
    [  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, ], # U
    [  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, ], # R
    [  0,  1,  0,  0,  0,  1,  0,  0,  1,  1,  0,  0, ], # F
    [  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, ], # D
    [  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, ], # L
    [  0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  1,  1, ], # B
    [  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, ], # M
    [  0,  1,  0,  1,  0,  1,  0,  1,  1,  1,  1,  1, ], # S
  ]
  N_PIECES = N_EDGES
  FACELETS_PER_PIECE = 2

  # flip: edge orientation coordinate [0, 2^11) (2048)

  def set_flip(self, flip: int) -> None:
    return self.set_ori(flip)

  def get_flip(self) -> int:
    return self.get_ori()

  # slice: E edge permutation coordinate [0, 12C4 * 4!) (11880)

  def set_slice(self, slice: int) -> None:
    return self.set_prm(slice, [FR, FL, BL, BR], right_handed=True)

  def get_slice(self) -> int:
    return self.get_prm([FR, FL, BL, BR], right_handed=True)

  # edgeU: Phase 1 U edge permutation coordinate [0, 12C3 * 3!) (1320)

  def set_edgeU(self, edgeU: int) -> None:
    return self.set_prm(edgeU, [UR, UF, UL])

  def get_edgeU(self) -> int:
    return self.get_prm([UR, UF, UL])

  # edgeD: Phase 1 D edge permutation coordinate [0, 12C3 * 3!) (1320)

  def set_edgeD(self, edgeD: int) -> None:
    return self.set_prm(edgeD, [UB, DR, DF])

  def get_edgeD(self) -> int:
    return self.get_prm([UB, DR, DF])

  # edgeUD: Phase 2 non-E edge permutation coordinate [0, 8C6 * 6!) (20160)

  def set_edgeUD(self, edgeUD: int) -> None:
    return self.set_prm(edgeUD, [UR, UF, UL, UB, DR, DF])

  def get_edgeUD(self) -> int:
    return self.get_prm([UR, UF, UL, UB, DR, DF])

################################################################################

MOVE_DIR =  Path('cache/move')
MERGE_DIR = Path('cache/merge')
PRUNE_DIR = Path('cache/prune')

def write_move_table(Cube: Cube, attr: str, N: int, *, p2_only=False) -> array:
  cube = Cube()
  get = getattr(cube, 'get_' + attr)
  set = getattr(cube, 'set_' + attr)
  table = array('h')

  for coordinate in range(N):
    set(coordinate)
    if not (coordinate + 1) & 127:
      print(f'move/{attr}: {coordinate + 1}/{N}', end='\r')
    assert get() == coordinate
    for move in range(N_MOVES):
      for i in range(3):
        cube @= move
        if p2_only and move != U and move != D and i != 1:
          table.append(-1)
        else:
          table.append(get())
      cube @= move
    assert get() == coordinate
  print(f'move/{attr}: {N}/{N}')

  with open(MOVE_DIR.joinpath(attr), 'wb') as file:
    table.tofile(file)
  return table

def write_merge_table(Cube: type[Cube],
    attr1: str, N1: int, attr2: str, N2: int, attr3: str) -> None:
  cube1 = Cube()
  cube2 = Cube()
  set1 = getattr(cube1, 'set_' + attr1)
  set2 = getattr(cube2, 'set_' + attr2)
  get1 = getattr(cube2, 'get_' + attr1)
  get2 = getattr(cube2, 'get_' + attr2)
  get3 = getattr(cube2, 'get_' + attr3)
  table = array('h')

  for coordinate1 in range(N1):
    set1(coordinate1)
    for coordinate2 in range(N2):
      set2(coordinate2)
      coordinate = N2 * coordinate1 + coordinate2
      if not (coordinate + 1) & 127:
        print(f'merge/{attr3}: {coordinate + 1}/{N1 * N2}', end='\r')

      for i, p1, p2 in zip(range(Cube.N_PIECES), cube1.prm, cube2.prm):
        if p1 != Cube.N_PIECES - 1:
          if p2 != Cube.N_PIECES - 1:
            break
          else:
            cube2.prm[i] = cube1.prm[i]
      else:
        assert get1() == coordinate1
        assert get2() == coordinate2
        table.append(get3())
        continue
      table.append(-1)
  print(f'merge/{attr3}: {N1 * N2}/{N1 * N2}')

  with open(MERGE_DIR.joinpath(attr3), 'wb') as file:
    table.tofile(file)

def write_cache() -> None:
  MOVE_DIR.mkdir(parents=True, exist_ok=True)
  MERGE_DIR.mkdir(parents=True, exist_ok=True)
  PRUNE_DIR.mkdir(parents=True, exist_ok=True)

  twist   = write_move_table(CornerCube, 'twist',  N_TWIST )
  flip    = write_move_table(EdgeCube,   'flip',   N_FLIP  )

  corner  = write_move_table(CornerCube, 'corner', N_CORNER)
  slice   = write_move_table(EdgeCube,   'slice',  N_SLICE )
  edgeU   = write_move_table(EdgeCube,   'edgeU',  N_EDGEU )
  edgeD   = write_move_table(EdgeCube,   'edgeD',  N_EDGED )
  edgeUD  = write_move_table(EdgeCube,   'edgeUD', N_EDGEUD, p2_only=True)

  write_merge_table(EdgeCube, 'edgeU', MN_EDGEU, 'edgeD', MN_EDGED, 'edgeUD')

  parity = array('h', [
    1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
  ])

  def get_prune(table: array, index: int) -> int:
    if index & 1:
      return (table[index // 2] >> 4) & 0x0F
    else:
      return (table[index // 2] >> 0) & 0x0F

  def set_prune(table: array, index: int, value: int) -> None:
    if index & 1:
      table[index // 2] &= 0x0F | (value << 4)
    else:
      table[index // 2] &= 0xF0 | (value << 0)

  depth, done = 0, 1
  prune_table = array('B', (0xFF for _ in range((N_SLICE1 * N_TWIST) // 2 + 1)))
  set_prune(prune_table, 0, 0)
  while done != N_SLICE1 * N_TWIST:
    for i in range(N_SLICE1 * N_TWIST):
      twist1, slice1 = divmod(i, N_SLICE1)
      if get_prune(prune_table, i) == depth:
        for j in range(N_MOVES * 3):
          twist2 = twist[twist1 * N_MOVES * 3 + j]
          slice2 = slice[slice1 * N_SLICE2 * N_MOVES * 3 + j] // N_SLICE2
          if get_prune(prune_table, N_SLICE1 * twist2 + slice2) == 0x0F:
            set_prune(prune_table, N_SLICE1 * twist2 + slice2, depth + 1)
            done += 1
            if not done & 127:
              print(f'prune/twist: {done}/{N_SLICE1 * N_TWIST}', end='\r')
    depth += 1
  print(f'prune/twist: {N_SLICE1 * N_TWIST}/{N_SLICE1 * N_TWIST}')
  with open(PRUNE_DIR.joinpath('twist'), 'wb') as file:
    prune_table.tofile(file)

  depth, done = 0, 1
  prune_table = array('B', (0xFF for _ in range((N_SLICE1 * N_FLIP) // 2)))
  set_prune(prune_table, 0, 0)
  while done != N_SLICE1 * N_FLIP:
    for i in range(N_SLICE1 * N_FLIP):
      flip1, slice1 = divmod(i, N_SLICE1)
      if get_prune(prune_table, i) == depth:
        for j in range(N_MOVES * 3):
          flip2 = flip[flip1 * N_MOVES * 3 + j]
          slice2 = slice[slice1 * N_SLICE2 * N_MOVES * 3 + j] // N_SLICE2
          if get_prune(prune_table, N_SLICE1 * flip2 + slice2) == 0x0F:
            set_prune(prune_table, N_SLICE1 * flip2 + slice2, depth + 1)
            done += 1
            if not done & 127:
              print(f'prune/flip: {done}/{N_SLICE1 * N_FLIP}', end='\r')
    depth += 1
  print(f'prune/flip: {N_SLICE1 * N_FLIP}/{N_SLICE1 * N_FLIP}')
  with open(PRUNE_DIR.joinpath('flip'), 'wb') as file:
    prune_table.tofile(file)

  depth, done = 0, 1
  prune_table = array('B', (0xFF for _ in range((N_SLICE2 * N_CORNER * N_PARITY) // 2)))
  set_prune(prune_table, 0, 0)
  while done != N_SLICE2 * N_CORNER * N_PARITY:
    for i in range(N_SLICE2 * N_CORNER * N_PARITY):
      parity1 = i % 2
      corner1, slice1 = divmod(i // 2, N_SLICE2)
      if get_prune(prune_table, i) == depth:
        for j in [0, 1, 2, 4, 7, 9, 10, 11, 13, 16]:
          parity2 = parity[parity1 * N_MOVES * 3 + j]
          corner2 = corner[corner1 * N_MOVES * 3 + j]
          slice2  = slice[slice1  * N_MOVES * 3 + j]
          if get_prune(prune_table, (N_SLICE2 * corner2 + slice2) * N_PARITY + parity2) == 0x0F:
            set_prune(prune_table, (N_SLICE2 * corner2 + slice2) * N_PARITY + parity2, depth + 1)
            done += 1
            if not done & 127:
              print(f'prune/corner: {done}/{N_SLICE2 * N_CORNER * N_PARITY}', end='\r')
    depth += 1
  print(f'prune/corner: {N_SLICE2 * N_CORNER * N_PARITY}/{N_SLICE2 * N_CORNER * N_PARITY}')
  with open(PRUNE_DIR.joinpath('corner'), 'wb') as file:
    prune_table.tofile(file)

  depth, done = 0, 1
  prune_table = array('B', (0xFF for _ in range((N_SLICE2 * N_EDGEUD * N_PARITY) // 2)))
  set_prune(prune_table, 0, 0)
  while done != N_SLICE2 * N_EDGEUD * N_PARITY:
    for i in range(N_SLICE2 * N_EDGEUD * N_PARITY):
      parity1 = i % 2
      edge1, slice1 = divmod(i // 2, N_SLICE2)
      if get_prune(prune_table, i) == depth:
        for j in [0, 1, 2, 4, 7, 9, 10, 11, 13, 16]:
          parity2 =  parity[parity1 * N_MOVES * 3 + j]
          edge2   = edgeUD[edge1   * N_MOVES * 3 + j]
          slice2  =  slice[slice1  * N_MOVES * 3 + j]
          if get_prune(prune_table, (N_SLICE2 * edge2 + slice2) * N_PARITY + parity2) == 0x0F:
            set_prune(prune_table, (N_SLICE2 * edge2 + slice2) * N_PARITY + parity2, depth + 1)
            done += 1
            if not done & 127:
              print(f'prune/edge: {done}/{N_SLICE2 * N_EDGEUD * N_PARITY}', end='\r')
    depth += 1
  print(f'prune/edge: {N_SLICE2 * N_EDGEUD * N_PARITY}/{N_SLICE2 * N_EDGEUD * N_PARITY}')
  with open(PRUNE_DIR.joinpath('edge'), 'wb') as file:
    prune_table.tofile(file)

if __name__ == '__main__':
  write_cache()
