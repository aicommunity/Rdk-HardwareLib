#include "HardwareWaveRoverControllerWidget.h"

#include <QVBoxLayout>

#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"
#include "widgets/HardwareWheeledDrivePanelWidget.h"

HardwareWaveRoverControllerWidget::HardwareWaveRoverControllerWidget(QWidget* parent,
                                                                     RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    Drive = new HardwareWheeledDrivePanelWidget(this);
    Drive->setMotorDriverVisible(false);
    FeedbackLabel = new QLabel(tr("LastFeedbackJson: —"), this);
    FeedbackLabel->setWordWrap(true);
    auto* feedbackPage = new QWidget(this);
    auto* fbLay = new QVBoxLayout(feedbackPage);
    HardwareGuiHelpers::applyCompactLayout(fbLay);
    fbLay->addWidget(FeedbackLabel);
    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    BoardPanel->setEsp32Mode(true);
    Tabs = new QTabWidget(this);
    Tabs->setDocumentMode(true);
    Tabs->addTab(Drive, tr("Drive"));
    Tabs->addTab(feedbackPage, tr("Feedback"));
    Tabs->addTab(BoardPanel, tr("Board"));
    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(Tabs);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareWaveRoverControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    Drive->setContext(context);
    BoardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareWaveRoverControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.waverover");
}

void HardwareWaveRoverControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;
    Drive->refreshFromModel();
    BoardPanel->refreshFromModel();
    FeedbackLabel->setText(tr("LastFeedbackJson: %1")
                               .arg(HardwareGuiHelpers::getProp(Context, "LastFeedbackJson")));
}
