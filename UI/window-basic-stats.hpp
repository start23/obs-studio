#pragma once

#include <obs.hpp>
#include <util/platform.h>
#include <QPointer>
#include <QDialog>
#include <QTimer>
#include <QLabel>
#include <QList>

class QGridLayout;

class OBSBasicStats : public QDialog {
	Q_OBJECT

	QLabel *cpuUsage = nullptr;
	QLabel *hddSpace = nullptr;
	QLabel *memUsage = nullptr;

	QLabel *renderTime = nullptr;
	QLabel *skippedFrames = nullptr;
	QLabel *missedFrames = nullptr;

	QGridLayout *outputLayout = nullptr;

	os_cpu_usage_info_t *cpu_info = nullptr;

	QTimer timer;

	struct OutputLabels {
		QPointer<QLabel> name;
		QPointer<QLabel> status;
		QPointer<QLabel> disconnections;
		QPointer<QLabel> droppedFrames;
		QPointer<QLabel> metabytesSent;
		QPointer<QLabel> bitrate;
	};

	QList<OutputLabels> outputLabels;

	void AddOutputLabels(QString name);
	void Update();

public:
	OBSBasicStats(QWidget *parent = nullptr);
	~OBSBasicStats();
};
