#include "window-basic-stats.hpp"
#include "obs-frontend-api/obs-frontend-api.h"
#include "obs-app.hpp"

#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <string>

OBSBasicStats::OBSBasicStats(QWidget *parent)
	: QDialog(parent)
{
	QHBoxLayout *mainLayout = new QHBoxLayout();
	QGridLayout *topLayout = new QGridLayout();
	outputLayout = new QGridLayout();

	int row = 0;

	auto newStat = [&] (const char *strLoc, QWidget *label, int col)
	{
		std::string str = "Basic.Stats.";
		str += strLoc;
		topLayout->addWidget(
				new QLabel(QTStr(str.c_str()), this),
				row, col);
		topLayout->addWidget(label, row++, col + 1);
	};

	/* --------------------------------------------- */

	cpuUsage = new QLabel(this);
	hddSpace = new QLabel(this);
#ifdef _WIN32
	memUsage = new QLabel(this);
#endif

	newStat("CPUUsage", cpuUsage, 0);
	newStat("HDDSpaceAvailable", hddSpace, 0);
#ifdef _WIN32
	newStat("MemoryUsage", memUsage, 0);
#endif

	renderTime = new QLabel(this);
	skippedFrames = new QLabel(this);
	missedFrames = new QLabel(this);
	row = 0;

	newStat("AverageTimeToRender", renderTime, 2);
	newStat("SkipppedFrames", skippedFrames, 2);
	newStat("MissedFrames", missedFrames, 2);

	/* --------------------------------------------- */

	QPushButton *closeButton = new QPushButton(QTStr("Close"));
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(closeButton);

	/* --------------------------------------------- */

	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Settings.Output"), this),
			0, 0);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.Status"), this),
			0, 0);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.Disconnections"), this),
			0, 0);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.DroppedFrames"), this),
			0, 0);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.MegabytesSent"), this),
			0, 0);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.Bitrate"), this),
			0, 0);

	/* --------------------------------------------- */

	AddOutputLabels(QTStr("Basic.Stats.Output.Stream"));
	AddOutputLabels(QTStr("Basic.Stats.Output.Recording"));

	/* --------------------------------------------- */

	QScrollArea *scrollArea = new QScrollArea(this);

	QVBoxLayout *outputContainerLayout = new QVBoxLayout();
	outputContainerLayout->addLayout(outputLayout);
	outputContainerLayout->addStretch();
	scrollArea->widget()->setLayout(outputContainerLayout);

	/* --------------------------------------------- */

	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(scrollArea);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);

	/* --------------------------------------------- */

	connect(closeButton, &QPushButton::clicked, [this] () {close();});

	installEventFilter(CreateShortcutFilter());

	resize(800, 200);
	setWindowTitle(QTStr("Basic.Stats"));
	setSizeGripEnabled(true);
	setWindowModality(Qt::NonModal);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

void OBSBasicStats::AddOutputLabels(QString name)
{
	OutputLabels ol;
	ol.name = new QLabel(name, this);
	ol.status = new QLabel(this);
	ol.disconnections = new QLabel(this);
	ol.droppedFrames = new QLabel(this);
	ol.metabytesSent = new QLabel(this);
	ol.bitrate = new QLabel(this);

	int col = 0;
	outputLayout->addWidget(ol.name, outputLabels.size(), col++);
	outputLayout->addWidget(ol.status, outputLabels.size(), col++);
	outputLayout->addWidget(ol.disconnections, outputLabels.size(), col++);
	outputLayout->addWidget(ol.droppedFrames, outputLabels.size(), col++);
	outputLayout->addWidget(ol.metabytesSent, outputLabels.size(), col++);
	outputLayout->addWidget(ol.bitrate, outputLabels.size(), col++);
	outputLabels.push_back(ol);
}
