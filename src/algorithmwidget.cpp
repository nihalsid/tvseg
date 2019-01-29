#include "algorithmwidget.h"

#include "boost/foreach.hpp"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QtConcurrent>

namespace {

void copyStringVector(const std::vector<std::string> &input, QVector<QString> &output)
{
    output.clear();
    std::transform(input.begin(), input.end(), std::back_inserter(output), &QString::fromStdString);
}

} // namespace


namespace tvseg_ui {

AlgorithmWidget::AlgorithmWidget(QWidget *parent) :
    QWidget(parent),
    display_(new CvImageDisplayWidget),
    sidePane_(new QVBoxLayout),
    actionsGroup_(new QGroupBox),
    groupLayout_(new QGridLayout),
    settingsEditor_(new SettingsEditor),
    computeButtonMapper_(new QSignalMapper(this)),
    clearButtonMapper_(new QSignalMapper(this)),
    showButtonMapper_(new QSignalMapper(this)),
    watcher_(new QFutureWatcher<void>(this)),
    runningAction_(-1),
    runAllButton_(new QPushButton),
    saveResultButton_(new QPushButton),
    computeMetricsButton_(new QPushButton),
    commandsGroup_(new QGroupBox),
    presetsGroup_(new QGroupBox),
    presetsComboBox_(new QComboBox),
    presetsSetButton_(new QPushButton)
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    setLayout(topLayout);
    topLayout->addWidget(display_, 1);
    topLayout->addLayout(sidePane_, 0);

    sidePane_->addWidget(commandsGroup_, 0);
    sidePane_->addWidget(actionsGroup_, 0);
    sidePane_->addWidget(presetsGroup_, 0);
    sidePane_->addWidget(settingsEditor_, 1);

    // FIXME: find out why margins generated by designer/uic tool does not correspond to defaults show in designer or used if nothing is set.
    topLayout->setContentsMargins(11,11,11,11);
    topLayout->setSpacing(6);

    actionsGroup_->setTitle("Actions");
    actionsGroup_->setLayout(groupLayout_);
    groupLayout_->setSpacing(6);

    commandsGroup_->setTitle("Commands");
    QVBoxLayout *commandsLayout = new QVBoxLayout;
    commandsLayout->setContentsMargins(11,11,11,11);
    commandsLayout->setSpacing(6);
    commandsGroup_->setLayout(commandsLayout);

    runAllButton_->setText("run all");
    commandsLayout->addWidget(runAllButton_);
    connect(runAllButton_, SIGNAL(clicked()), this, SIGNAL(runAll()));

    saveResultButton_->setText("save result");
    commandsLayout->addWidget(saveResultButton_);
    connect(saveResultButton_, SIGNAL(clicked()), this, SIGNAL(saveResult()));

    computeMetricsButton_->setText("compute metrics");
    commandsLayout->addWidget(computeMetricsButton_);
    connect(computeMetricsButton_, SIGNAL(clicked()), this, SIGNAL(computeMetrics()));

    connect(computeButtonMapper_, SIGNAL(mapped(int)), this, SLOT(computeButtonClicked(int)));
    connect(clearButtonMapper_, SIGNAL(mapped(int)), this, SLOT(clearButtonClicked(int)));
    connect(showButtonMapper_, SIGNAL(mapped(int)), this, SLOT(showButtonClicked(int)));
    connect(watcher_, SIGNAL(started()), this, SIGNAL(algorithmStarted()));
    connect(watcher_, SIGNAL(finished()), this, SIGNAL(algorithmStopped()));
    connect(watcher_, SIGNAL(finished()), this, SLOT(algorithmCompleted()));
    connect(settingsEditor_, SIGNAL(valueChanged(QtProperty*,QVariant)), this, SLOT(settingsValueChanged(QtProperty*,QVariant)));

    presetsGroup_->setTitle("Setting presets");
    QHBoxLayout *presetsLayout = new QHBoxLayout;
    presetsLayout->setContentsMargins(11,11,11,11);
    presetsLayout->setMargin(6);
    presetsGroup_->setLayout(presetsLayout);
    presetsLayout->addWidget(presetsComboBox_, 1);
    presetsLayout->addWidget(presetsSetButton_, 0);

    presetsComboBox_->addItem("<select>");

    presetsSetButton_->setText("set");
    connect(presetsSetButton_, SIGNAL(clicked()), this, SLOT(setPresetClicked()));
}

void AlgorithmWidget::addAlgorithm(const tvseg::CVAlgorithmInterface *algorithmInterface)
{
    // save a copy of the algorithm interface
    const int algoId = algorithms_.size();
    algorithms_.push_back(tvseg::CVAlgorithmInterfaceBase(algorithmInterface));

    // first id of actions
    int actionId = actions_.size();

    // setup settings editor
    tvseg::settings::SettingsPtr settings = algorithmInterface->settings();
    if (settings) {
        // TODO: remove `settingsInSetup_` and make sure settingseditor only send value changed when settings actually
        //       changed, not when edit was attempted.
        copyStringVector(settings->names(), settingsInSetup_);
        QVector<QtProperty*> addedProperties = settingsEditor_->addEntries(*settings);
        foreach (QtProperty* p, addedProperties) {
            if (p) {
                propertyMap_[p] = QPair<int,int>(algoId, actionId);
            }
        }
        settingsInSetup_.clear();
    }

    // setup action buttons
    BOOST_FOREACH(const tvseg::CVAlgorithmInterface::Action &action, algorithmInterface->actions()) {
        actions_.push_back(action);

        QLabel *label = new QLabel;
        label->setText(QString::fromStdString(action.name));

        QPushButton *computeButton = new QPushButton;
        computeButton->setText("c");
        connect(computeButton, SIGNAL(clicked()), computeButtonMapper_, SLOT(map()));
        computeButtonMapper_->setMapping(computeButton, actionId);

        QPushButton *showButton = new QPushButton;
        showButton->setText("s");
        connect(showButton, SIGNAL(clicked()), showButtonMapper_, SLOT(map()));
        showButtonMapper_->setMapping(showButton, actionId);
        showButtons_.push_back(showButton);
        showButton->setEnabled(action.resultAvailable());

        QPushButton *clearButton = new QPushButton;
        clearButton->setText("x");
        connect(clearButton, SIGNAL(clicked()), clearButtonMapper_, SLOT(map()));
        clearButtonMapper_->setMapping(clearButton, actionId);
        clearButtons_.push_back(clearButton);
        clearButton->setEnabled(action.resultAvailable());

        groupLayout_->addWidget(label, actionId, 0);
        groupLayout_->addWidget(computeButton, actionId, 1);
        groupLayout_->addWidget(showButton, actionId, 2);
        groupLayout_->addWidget(clearButton, actionId, 3);

        ++actionId;
    }
}

void AlgorithmWidget::clear()
{
    propertyMap_.clear();
    algorithms_.clear();
    settingsEditor_->clear();
    clearActions();
}

void AlgorithmWidget::updateFromSettings()
{
    settingsEditor_->updateFromSettings();
}

void AlgorithmWidget::addPreset(QString name)
{
    presetsComboBox_->addItem(name);
}

void AlgorithmWidget::computeButtonClicked(int id)
{
    if (id < 0 || id >= actions_.size()) {
        std::cout << "Invalid action id " << id << ". Ignore button click.";
        return;
    }
    runningAction_ = id;
    QFuture<void> future = QtConcurrent::run(actions_[id].computeFn);
    watcher_->setFuture(future);
}

void AlgorithmWidget::clearButtonClicked(int id)
{
    if (id < 0 || id >= actions_.size()) {
        std::cout << "Invalid action id " << id << ". Ignore button click.";
        return;
    }
    actions_[id].clear();
    display_->clearImage();
    clearButtons_[id]->setEnabled(false);
    showButtons_[id]->setEnabled(false);
}

void AlgorithmWidget::showButtonClicked(int id)
{
    if (id < 0 || id >= actions_.size()) {
        std::cout << "Invalid action id " << id << ". Ignore button click.";
        return;
    }

    const tvseg::CVAlgorithmInterface::Action &action = actions_[id];
    if (action.resultAvailable()) {
        display_->setImage(action.result());
        clearButtons_[id]->setEnabled(true);
        showButtons_[id]->setEnabled(true);
    } else {
        clearButtons_[id]->setEnabled(false);
        showButtons_[id]->setEnabled(false);
        std::cout << "Result for action '" << action.name << "' not available. Cannot display.";
    }
}

void AlgorithmWidget::settingsValueChanged(QtProperty *property, QVariant /*value*/)
{
    if (property) {
        if (!propertyMap_.contains(property)) {
            if (!settingsInSetup_.contains(property->propertyName())) {
                std::cout << "AlgorithmWidget: Got valueChanged signal for unregistered property '" << property->propertyName().toStdString() << "' with value '" << property->displayText().toStdString() << "'";
            }
            return;
        }

        const int algoId   = propertyMap_[property].first;

        if (0 > algoId || algoId >= algorithms_.size()) {
            std::cout << "AlgorithmWidget: Invalid algorithm id '" << algoId << "' registered for property '" << property->propertyName().toStdString() << "' with value '" << property->displayText().toStdString() << "'";
            return;
        }

        tvseg::settings::SettingsConstPtr s = algorithms_[algoId].settings();
        std::string name = "autoRecompute";
        tvseg::settings::EntryConstPtr e = s->getEntry(name);
        if (e) {
            boost::any value = e->value();
            const bool* autoCompute = boost::any_cast<bool>(&value);
            if (!autoCompute) {
                std::cout << "AlgorithmWidget: autoRecompute setting not boolean.";
                return;
            }
            if (*autoCompute) {
                const int actionId = propertyMap_[property].second;

                if (actionId < 0 || actionId >= actions_.size()) {
                    std::cout << "AlgorithmWidget: Invalid action id " << actionId << ".";
                    return;
                }

                // if result was computed before, recompute it.
                if (actions_[actionId].resultAvailable()) {
                    computeButtonClicked(actionId);
                }
            }
        }
    }
}

void AlgorithmWidget::algorithmCompleted()
{
    showButtonClicked(runningAction_);
}

void AlgorithmWidget::setPresetClicked()
{
    if (presetsComboBox_->currentIndex() >= 1) {
        QString name = presetsComboBox_->currentText();
        presetsComboBox_->setCurrentIndex(0);
        emit setPreset(name);
    }
}

void AlgorithmWidget::clearActions()
{
    showButtons_.clear();
    clearButtons_.clear();
    actions_.clear();
    for (QLayoutItem* item = actionsGroup_->layout()->itemAt(0); item != NULL; item = actionsGroup_->layout()->itemAt(0)) {
        QWidget *w = item->widget();
        if (w) {
            delete w;
        }
        delete item;
    }
}



} // namespace tvseg_ui
