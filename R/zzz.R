## Copyright (C) 2013-2015 Kasper Kristensen
## License: GPL-2

## .First.lib <- function(lib, pkg) {
##   library.dynam("TMB", pkg, lib)
## }

## Matrix package versions that are likely to break ABI compatibility:
MVI <- c("1.6-2", "1.7-0")
## findInterval
findMVI <- function(v) {
    sum(MVI <= numeric_version(v))
}
sameMVI <- function(x, y) {
    findMVI(x) == findMVI(y)
}

checkMatrixPackageVersion <- function(warn=TRUE) {
    ## It is unsafe to use the TMB package with versions of 'Matrix'
    ## other than the one TMB was originally built with.
    file <- paste0(system.file(package="TMB"),"/Matrix-version")
    cur.Matrix.version <- as.character(packageVersion("Matrix"))
    if(!file.exists(file)) {
        writeLines(cur.Matrix.version, con = file)
    }
    TMB.Matrix.version <- readLines(file)
    if(warn && !sameMVI(TMB.Matrix.version, cur.Matrix.version)) {
        warning(
            "Package version inconsistency detected.\n",
            "TMB was built with Matrix version ",
            TMB.Matrix.version,
            "\n",
            "Current Matrix version is ",
            cur.Matrix.version,
            "\n",
            "Please re-install 'TMB' from source using install.packages('TMB', type = 'source') ",
            "or ask CRAN for a binary version of 'TMB' matching CRAN's 'Matrix' package"
        )
    }
}

.onLoad <- function(lib, pkg) {
    library.dynam("TMB", pkg, lib)
    checkMatrixPackageVersion(getOption("TMB.check.Matrix", TRUE))
    ## Select AD framework (CppAD or TMBad) used by TMB::compile
    tmb.ad.framework <- getOption("tmb.ad.framework", NULL)
    if (is.null(tmb.ad.framework))
        tmb.ad.framework <- Sys.getenv("TMB_AD_FRAMEWORK", "CppAD")
    options("tmb.ad.framework" = tmb.ad.framework)
}

.onUnload <- function(libpath) {
    library.dynam.unload("TMB", libpath)
}

## .LastLib <- function(libpath)
## {
##   library.dynam.unload("TMB", libpath)
## }


.onAttach <- function(lib, pkg) {
  exfolder <- system.file("examples", package = "TMB")
  dll <- paste0(exfolder, Sys.getenv("R_ARCH"), "/simple", .Platform$dynlib.ext)
  if(!file.exists(dll)) runExample("simple", dontrun=TRUE, eigen.disable.warnings=FALSE)
}
