library(dplyr, warn.conflicts=F)
library(xtable)
library(ggplot2)
library(scales)
library(tidyr)
library(data.table)
options(width=300)
options(xtable.caption.placement='top')
options(xtable.booktabs=T)
options(xtable.table.placement='t')
options(xtable.include.rownames=F)
options(xtable.include.colnames=F)
options(xtable.math.style.exponents=T)
options(scipen=999)
options(dplyr.summarise.inform = FALSE)

# Save .tex file
bkvChroma = read.table('bkv.dat', h=T)

# norefs = bkvChroma %>% 
#   subset(select=-c(source_omega_lb, source_omega_ub, source_chi_lb, source_chi_ub))
# justrefs = bkvChroma %>% 
#   subset(select=c(instance,source_omega_lb, source_omega_ub, source_chi_lb, source_chi_ub))

# options(xtable.include.colnames=F)
# norefsX = xtable(norefs, digits=c(0, 0, 0, 0, 0, 0, 0, 0), label="tab:norefs")
# atr = list()
# atr$pos = list(0)
# atr$command = c('Instance & n & m & $\\ubar{\\omega}$ & $\\bar{\\omega}$ & $\\ubar{\\chi}$ & $\\bar{\\chi}$ \\\\')
# lines = print(norefsX, size='\\footnotesize', add.to.row=atr, hline.after=c(-1,0,nrow(norefs)), sanitize.text.function=identity,
#               NA.string='---', floating=F, tabular.environment="longtable", print.results=F)
# lines = gsub('_', '\\\\_', lines)
# writeLines(lines, con="tablenoref.tex")

# options(xtable.include.colnames=F)
# justrefsX = xtable(justrefs, digits=c(0, 0, 0, 0, 0, 0), label="tab:justrefs")
# atr = list()
# atr$pos = list(0)
# atr$command = c('Instance & ref. $\\ubar{\\omega}$ & ref. $\\bar{\\omega}$ & ref. $\\ubar{\\chi}$ & ref. $\\bar{\\chi}$ \\\\')
# lines = print(justrefsX, size='\\footnotesize', add.to.row=atr, hline.after=c(-1,0,nrow(justrefs)), sanitize.text.function=identity,
#               NA.string='---', floating=F, tabular.environment="longtable", print.results=F)
# lines = gsub('_', '\\\\_', lines)
# writeLines(lines, con="tablejustref.tex")

options(xtable.include.colnames=F)
bkvChromaX = xtable(bkvChroma, digits=c(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), label="tab:instances")
atr = list()
atr$pos = list(0)
atr$command = c('Instance & n & m & $\\ubar{\\omega}$ & ref. & $\\bar{\\omega}$ & ref. & $\\ubar{\\chi}$ & ref. & $\\bar{\\chi}$ & ref. \\\\')
lines = print(bkvChromaX, size='\\footnotesize', add.to.row=atr, hline.after=c(-1,0,nrow(bkvChroma)), sanitize.text.function=identity,
              NA.string='---', floating=F, tabular.environment="longtable", print.results=F)
lines = gsub('_', '\\\\_', lines)
writeLines(lines, con="table.tex")