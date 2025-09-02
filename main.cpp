#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include "MainWindow.h"
#include "translations.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Initialize translations
    ensureTranslationsInitialized();
    
    // Set application metadata for command line parser
    app.setApplicationName(Translations::tr("WINDOW_TITLE"));
    app.setApplicationVersion("1.6");
    app.setApplicationDisplayName(Translations::tr("WINDOW_TITLE"));
    
    // Create command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(Translations::tr("WINDOW_TITLE"));
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add positional argument for section
    parser.addPositionalArgument("section", Translations::tr("WINDOW_TITLE"), "[section]");
    
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