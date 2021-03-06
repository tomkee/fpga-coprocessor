module Parser.AST where

import Data.Default
import Data.Int      (Int16)


type Module = [Expr]

data Expr = Lit Int16
          | VecLit [Int16]
          | VarE VarName
          | BinOp Op Expr Expr
          | If Expr [Expr] [Expr]
          | Assign VarName Expr
          | Decl VarName Type Expr
          | Loop Expr [Expr]
          | Rot Expr
          deriving (Show, Eq, Ord)


type VarName = String
data Type    = Scalar | Vector Int16 deriving (Show, Eq, Ord)

--        +          -          *          /         %          &           ?
data Op = Add Type | Sub Type | Mul Type | Div Type | Mod Type | DotPr Type deriving (Show, Eq, Ord)


instance Default Type where
    def = Scalar


------- UTILS --------
loopIf :: Int -> Expr -> (Expr, Expr, Expr)
loopIf lab cond =
    ( Decl varName Scalar cond                                   -- loop's stopping condition
    , Assign varName (BinOp (Sub Scalar) (VarE varName) (Lit 1)) -- decrement counter
    , VarE varName                                               -- evaluate counter
    ) where varName = "counter" ++ show lab


opLen :: Op -> Int
opLen op = case op of
    Add Scalar       -> 1
    Sub Scalar       -> 1
    Mul Scalar       -> 1
    Div Scalar       -> 1
    Add (Vector n)   -> fromIntegral n
    Sub (Vector n)   -> fromIntegral n
    Mul (Vector n)   -> fromIntegral n
    Div (Vector n)   -> fromIntegral n
    Mod (Vector n)   -> fromIntegral n
    DotPr (Vector n) -> fromIntegral n
    _              -> -1

isScalarOp :: Op -> Bool
isScalarOp op = case op of
    Add Scalar     -> True
    Sub Scalar     -> True
    Mul Scalar     -> True
    Div Scalar     -> True
    DotPr Scalar   -> True
    _              -> False


isVectorOp :: Op -> Bool
isVectorOp = not . isScalarOp
