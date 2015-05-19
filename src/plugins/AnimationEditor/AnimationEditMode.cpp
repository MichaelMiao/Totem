#include "AnimationEditMode.h"
#include "AniMainWindow.h"
#include <QVBoxLayout>
#include <QPushButton>

#define ID_ANIMATION_MODE "AnimationEdit"
AnimationEditMode::AnimationEditMode()
{
	setObjectName(QLatin1String("AnimationEdit"));
	setDisplayName(tr("AnimationEdit"));
	setIcon(QIcon(QLatin1String(":/AnimationEditor/Resources/AnimateEdit.png")));
	//����ģʽ���ȼ�
	setId(ID_ANIMATION_MODE);
	setType(ID_ANIMATION_MODE);

	AniMainWindow* pAniMain = new AniMainWindow(0);
	setWidget(pAniMain);
}


AnimationEditMode::~AnimationEditMode()
{
}
