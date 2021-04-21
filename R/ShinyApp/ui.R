library(shiny)
# runExample("01_hello")

# Define UI for app that draws a histogram ----
ui <- fluidPage(

  # App title ----
  titlePanel("Hello Shiny!"),

  # Main panel for displaying outputs ----
  mainPanel(

    # Output: Histogram ----
    plotOutput(outputId = "distPlot")

  )

)
