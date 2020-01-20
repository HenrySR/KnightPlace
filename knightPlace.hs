import Data.List

--queens n = solve n
--       where
--       solve 0 = [ [] ]
--      solve k = [q : b | b <- solve (k-1), q <- [0..(n-1)], safe q b ]
--       safe q b = and [not (checks q b i) | i <-[0..(length b - 1) ] ]
--       checks q b i = q == b!!i || abs (q-b!!i) == i+1
     
     
--make board based on the length of the input
makeBoard :: [[Int]] -> Int -> [[Int]]
makeBoard x length = if length > 0
then makeBoard ([]:x) (length-1)
else x       
       
--queen positions in a list containing their location as a tuple of (x,y)
queenInList :: [Int] -> Int -> [(Int, Int)]
queenInList [] index = []
queenInList (x:xs) index | x > 0 = (index,x):queenInList xs (index+1) | otherwise = queenInList xs (index+1)

--qhorizontal creates a list with the horizontal queen movement
qHorizonatal:: Int -> (Int,Int) -> [(Int,Int)]
qHorizonatal length (x,y) = [(x+a, y) | a<-[(-length)..length], x+a>0, x+a<=length]

--qVertical creates a list with the vertical queen movement                        
qVertical:: Int -> (Int,Int) -> [(Int,Int)]
qVertical length (x,y) = [(x, y+b) | b<-[(-length)..length], y+b>0, y+b<=length]                      
--qDiagonal creates a list with the diagonal queen movement
qDiagonal:: Int -> (Int,Int) -> [(Int,Int)]
qDiagonal length (x,y) = [(x+a, y+b) | a<-[(-length)..length], b<-[(-length)..length], x+a>0, y+b>0, x+a<=length, y+b<=length,abs a == abs b]


--qKLMoves queen knight L shaped moves
qKLMoves:: Int -> (Int,Int) -> [(Int,Int)]
qKLMoves length (x,y) = [(x+a, y+b) | a<-[-2,-1,1,2], b<-[-2,-1,1,2], x+a>0, y+b>0, x+a<=length, y+b<=length,abs a /= abs b]

--fills the empty lists with 0 to match specifications
fillEmpty :: [[Int]] -> [[Int]] -> Int -> [[Int]]
fillEmpty [] list index = list
fillEmpty (x:xs) list index = if length x > 0 
  then fillEmpty xs list (index+1) 
  else fillEmpty xs (replaceAt index (0:x) list) (index+1)
    
replaceAt :: Int -> a -> [a] -> [a]
replaceAt n item ls = a ++ (item:b) where (a, (_:b)) = splitAt n ls
    
aggregate :: [[i]] -> [i]
aggregate [] = []  
aggregate (x:xs) = x ++ aggregate xs

--creates a list with all possible queen moves and where a knight could take the queen from
--because they count as the same thing in this context
possibleQMoves:: [(Int, Int)] -> Int -> [(Int, Int)]
possibleQMoves qLocations length = nub(aggregate(map(qDiagonal length) qLocations ++ map(qHorizonatal length) qLocations ++ map(qVertical length) qLocations ++ map(qKLMoves length) qLocations))

safeCoords :: [(Int, Int)] -> Int -> [(Int, Int)]
safeCoords unsafeCoords length = [(x,y) | x <- [1..length], y <- [1..length], (x,y) `notElem` unsafeCoords]

output :: [(Int, Int)] -> [[Int]] -> [[Int]]
output [] list = list
output (x:xs) list = output xs (replaceAt (fst x-1) ((list!!(fst x-1)) ++ [snd x])list)

knightPlace :: [Int] -> [[Int]]
knightPlace input 
  | length input == 0 = []
  | otherwise = map sort knights
  where {knights = fillEmpty knightLocations knightLocations 0;
      knightLocations = output (safeCoords (possibleQMoves qLocations (length input)) (length input)) board;
      qLocations = queenInList input 1;
      board = makeBoard [] (length input);
      } 
