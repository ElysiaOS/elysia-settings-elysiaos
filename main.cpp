#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata for command line parser
    app.setApplicationName("Elysia Settings");
    app.setApplicationVersion("1.6");
    app.setApplicationDisplayName("Elysia Settings");
    
    // Create command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Elysia OS Settings Application");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add positional argument for section
    parser.addPositionalArgument("section", "Settings section to open directly (optional)", "[section]");
    
    // Parse command line arguments
    parser.process(app);
    
    // Get positional arguments
    const QStringList args = parser.positionalArguments();
    
    MainWindow w;
    
    // If a section is specified, launch directly to that section
    if (!args.isEmpty()) {
        QString section = args.first().toLower();
        w.launchSection(section);
    }
    
    w.show();
    return app.exec();
}