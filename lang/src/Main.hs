module Main where


import Parser.Parser       (parseFile)
import CodeGen.Generator   (symTable, modAST, asmCode, runASTTranslation, avAddrs, constData)
import CodeGen.ASM         (makeASM)
import CodeGen.LabelRename (renameLabels)
import CodeGen.Typechecker (runTypechecker, TypecheckerError, err)
import CodeGen.Binary      (genBinary)

import Text.Show.Pretty  (ppShow)
import Control.Lens
import Control.Monad     ((<=<))


runCompiler ast = case runTypechecker ast of
    Right newAst -> runASTTranslation newAst
    Left  tcerr  -> error $ "Typechecker error:\n    " ++ tcerr ^. err


main :: IO ()
main = do
    result <- parseFile "example"
    print "=========== AST ================"
    putStrLn . ppShow $ result
    print "========= Typechecking ========="
    putStrLn . ppShow $ runTypechecker result
    print "=========== ASMGen ============="
    let genData = runCompiler result
    -- putStrLn .               show . view symTable $ genData
    -- putStrLn .               show . view avAddrs  $ genData
    putStrLn . unlines . map show . view asmCode   $ genData
    putStrLn . unlines . map show . view constData $ genData
    genBinary "binarka" (view asmCode $ genData) (view constData $ genData)
    -- print "=========== ASM generation ============="
    -- putStrLn code1
    -- print "=========== Label renaming ============="
    -- putStrLn code2
