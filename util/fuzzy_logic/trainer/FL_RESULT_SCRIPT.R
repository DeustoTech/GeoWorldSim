 
args = commandArgs(trailingOnly=TRUE)

data = read.csv( args[1] , sep = ';' )

plot( y = (1-data$FL.CAR / data$REAL.CAR)*100 , x = data$STEP , ylim=c(0,100) , type = 'l' , col="red")
lines( y = (1-data$FL.BICYCLE / data$REAL.BICYCLE)*100 , x = data$STEP , type = 'l', col="blue")
lines( y = (1-data$FL.MOTORCYCLE / data$REAL.MOTORCYCLE)*100 , x = data$STEP , type = 'l', col="green")
lines( y = (1-data$FL.TRANSIT / data$REAL.TRANSIT)*100 , x = data$STEP , type = 'l', col="yellow")
lines( y = (1-data$FL.WALK / data$REAL.WALK)*100 , x = data$STEP , type = 'l', col="orange")

lines( y = data$FITNESS*20 , x = data$STEP , type = 'l', col="black")

legend('bottomleft' , c("ca","bi","mo","tr","wa","fit") , col=c("red","blue","green","yellow","orange","black") , lty=c(1,1) )