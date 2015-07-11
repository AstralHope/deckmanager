#include "cardfilter.h"
#include "iconbutton.h"
#include "config.h"
#include "limitcards.h"
#include "range.h"
#include "wrapper.h"

const quint32 CardFilter::cardTypes[] =
{
    Const::TYPE_MONSTER,
    Const::TYPE_SPELL,
    Const::TYPE_TRAP
};

const quint32 CardFilter::monsterTypes[] =
{
    Const::TYPE_NORMAL,
    Const::TYPE_EFFECT,
    Const::TYPE_FUSION,
    Const::TYPE_RITUAL,
    Const::TYPE_SPIRIT,
    Const::TYPE_UNION,
    Const::TYPE_DUAL,
    Const::TYPE_TUNER,
    Const::TYPE_SYNCHRO,
    Const::TYPE_FLIP,
    Const::TYPE_TOON,
    Const::TYPE_XYZ,
    Const::TYPE_PENDULUM,
    Const::TYPE_SYNCHRO | Const::TYPE_TUNER,
    Const::TYPE_NORMAL | Const::TYPE_TUNER
};

const quint32 CardFilter::spellTypes[] =
{
    Const::TYPE_QUICKPLAY,
    Const::TYPE_CONTINUOUS,
    Const::TYPE_EQUIP,
    Const::TYPE_FIELD,
    Const::TYPE_RITUAL
};

const quint32 CardFilter::trapTypes[] =
{
    Const::TYPE_CONTINUOUS,
    Const::TYPE_COUNTER
};

const quint32 CardFilter::monsterRaces[] =
{
    Const::RACE_WARRIOR,
    Const::RACE_SPELLCASTER,
    Const::RACE_FAIRY,
    Const::RACE_FIEND,
    Const::RACE_ZOMBIE,
    Const::RACE_MACHINE,
    Const::RACE_AQUA,
    Const::RACE_PYRO,
    Const::RACE_ROCK,
    Const::RACE_WINDBEAST,
    Const::RACE_PLANT,
    Const::RACE_INSECT,
    Const::RACE_THUNDER,
    Const::RACE_DRAGON,
    Const::RACE_BEAST,
    Const::RACE_BEASTWARRIOR,
    Const::RACE_DINOSAUR,
    Const::RACE_FISH,
    Const::RACE_SEASERPENT,
    Const::RACE_REPTILE,
    Const::RACE_PSYCHO,
    Const::RACE_DEVINE,
    Const::RACE_CREATORGOD,
    Const::RACE_PHANTOMDRAGON
};
const quint32 CardFilter::monsterAttrs[] =
{
    Const::ATTRIBUTE_DARK,
    Const::ATTRIBUTE_LIGHT,
    Const::ATTRIBUTE_WATER,
    Const::ATTRIBUTE_FIRE,
    Const::ATTRIBUTE_EARTH,
    Const::ATTRIBUTE_WIND,
    Const::ATTRIBUTE_DEVINE
};


CardFilter::CardFilter(QWidget *parent) : QWidget(parent)
{
    auto grid = new QGridLayout;
    cardType = new QComboBox;
    cardType->setEditable(false);
    cardType->addItem(tr("N/A"), QVariant(-1));
    for(auto t : cardTypes)
    {
        cardType->addItem(cardPool->getType(t), t);
    }

    cardTypeSub = new QComboBox;
    cardTypeSub->setEditable(false);
    cardTypeSub->addItem(tr("N/A"), QVariant(-1));


    int y = 0;

    grid->addWidget(cardType, y, 0);
    grid->addWidget(cardTypeSub, y, 1);


    auto tab = new QTabWidget;
    auto gridM = new QGridLayout;

    auto passL = new QLabel(config->getStr("label", "pass", "卡密"));
    auto raceL = new QLabel(config->getStr("label", "race", "种族"));
    auto attrL = new QLabel(config->getStr("label", "attr", "属性"));
    auto atkL = new QLabel(config->getStr("label", "atk", "攻击"));
    auto defL = new QLabel(config->getStr("label", "def", "守备"));
    auto levelL = new QLabel(config->getStr("label", "level", "等级"));
    auto rankL = new QLabel(config->getStr("label", "rank", "阶级"));
    auto scaleL = new QLabel(config->getStr("label", "scale", "刻度"));


    auto setL = new QLabel(config->getStr("label", "setcode", "系列"));
    auto nameL = new QLabel(config->getStr("label", "keyword", "关键字"));

    cardRace = new QComboBox;
    cardRace->setEditable(false);
    cardRace->addItem(tr("N/A"), QVariant(-1));
    cardAttr = new QComboBox;
    cardAttr->setEditable(false);
    cardAttr->addItem(tr("N/A"), QVariant(-1));

    passEdit = new QLineEdit;
    atkEdit = new QLineEdit;
    defEdit = new QLineEdit;
    levelEdit = new QLineEdit;
    rankEdit = new QLineEdit;
    scaleEdit = new QLineEdit;


    setEdit = new QLineEdit;
    nameEdit = new QLineEdit;

    y++;
    grid->addWidget(passL, y, 0);
    grid->addWidget(passEdit, y, 1);

    y++;
    auto hbox1 = new QHBoxLayout;
    limit = new QComboBox;
    limit->addItem("N/A", -1);
    limit->addItem(config->getStr("label", "banned", "禁止"), 0);
    limit->addItem(config->getStr("label", "limited", "限制"), 1);
    limit->addItem(config->getStr("label", "semi-limited", "准限制"), 2);
    limit->addItem(config->getStr("label", "nolimited", "无限制"), 3);
    hbox1->addWidget(limit);

    ot = new QComboBox;
    ot->addItem("N/A", 0);
    ot->addItem("OCG", 1);
    ot->addItem("TCG", 2);
    hbox1->addWidget(ot);

    grid->addLayout(hbox1, y, 0, 1, 2);


    int y1 = y;
    y1++;
    gridM->addWidget(raceL, y1, 0);
    gridM->addWidget(cardRace, y1, 1);

    y1++;
    gridM->addWidget(attrL, y1, 0);
    gridM->addWidget(cardAttr, y1, 1);

    y1++;
    gridM->addWidget(atkL, y1, 0);
    gridM->addWidget(atkEdit, y1, 1);

    y1++;
    gridM->addWidget(defL, y1, 0);
    gridM->addWidget(defEdit, y1, 1);

    y1++;
    gridM->addWidget(levelL, y1, 0);
    gridM->addWidget(levelEdit, y1, 1);

    y1++;
    gridM->addWidget(rankL, y1, 0);
    gridM->addWidget(rankEdit, y1, 1);

    y1++;
    gridM->addWidget(scaleL, y1, 0);
    gridM->addWidget(scaleEdit, y1, 1);

    auto wM = new QWidget;
    wM->setLayout(gridM);
    tab->addTab(wM, config->getStr("label", "attribute", "属性"));

    auto gridE = new QGridLayout;
    for(int i : range(32))
    {
        effects[i] = new QCheckBox;
        effects[i]->setText(config->getStr("effect", QString::number(i), ""));
        gridE->addWidget(effects[i], i / 2, i % 2);
    }

    auto wE = new QScrollArea;
    wE->setLayout(gridE);
    tab->addTab(wE, config->getStr("label", "effect", "效果"));

    y++;
    grid->addWidget(tab, y, 0, 4, 2);

    y += 4;
    grid->addWidget(setL, y, 0);
    grid->addWidget(setEdit, y, 1);
    y++;
    grid->addWidget(nameL, y, 0);
    grid->addWidget(nameEdit, y, 1);

    auto searchButton = new IconButton(":/icons/searchall.png", config->getStr("action", "searchall", "搜索"));
    auto searchThisButton = new IconButton(":/icons/search.png", config->getStr("action", "searchthis", "在结果中搜索"));
    auto searchDeckButton = new IconButton(":/icons/search.png", config->getStr("action", "searchdeck", "在卡组中搜索"));
    auto revertButton = new IconButton(":/icons/revert.png", config->getStr("action", "revert", "复位"));
    y++;
    grid->addWidget(searchButton, y, 0, 1, 2);

    auto hbox = new QHBoxLayout;
    hbox->addWidget(revertButton);
    hbox->addWidget(searchDeckButton);
    hbox->addWidget(searchThisButton);

    y++;
    grid->addLayout(hbox, y, 0, 1, 2);
    setLayout(grid);

    connect(cardType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CardFilter::setCardTypeSub);
    connect(searchButton, &QPushButton::clicked, this, &CardFilter::searchAll);
    connect(searchDeckButton, &QPushButton::clicked, this, &CardFilter::searchDeck);
    connect(searchThisButton, &QPushButton::clicked, this, &CardFilter::searchThis);
    connect(revertButton, &QPushButton::clicked, this, &CardFilter::revert);
}

void CardFilter::setCardTypeSub(int index)
{
    cardTypeSub->clear();
    cardTypeSub->addItem(tr("N/A"), ~0U);
    cardAttr->clear();
    cardAttr->addItem(tr("N/A"), ~0U);
    cardRace->clear();
    cardRace->addItem(tr("N/A"), ~0U);

    switch(index)
    {
    case 0:
        break;
    case 1:
        for(auto t : monsterTypes)
        {
            cardTypeSub->addItem(cardPool->getType(t), QVariant(t));
        }
        for(auto t : monsterAttrs)
        {
            cardAttr->addItem(cardPool->getAttr(t), QVariant(t));
        }
        for(auto t : monsterRaces)
        {
            cardRace->addItem(cardPool->getRace(t), QVariant(t));
        }
        break;
    case 2:
        cardTypeSub->addItem(config->getStr("string", "TYPE_NORMAL", "通常"), 0);
        for(auto t : spellTypes)
        {
            cardTypeSub->addItem(cardPool->getType(t), QVariant(t));
        }
        break;
    case 3:
        cardTypeSub->addItem(config->getStr("string", "TYPE_NORMAL", "通常"), 0);
        for(auto t : trapTypes)
        {
            cardTypeSub->addItem(cardPool->getType(t), QVariant(t));
        }
        break;
    default:
        break;
    }
}

void CardFilter::searchSet(quint32 id)
{
    call_with_ref([&](Card &card) {
        quint64 setcode1 = card.setcode;

        auto &map = cardPool->getMap();
        auto ls = Type::DeckP::create();
        for(auto &it : map)
        {
            quint64 set_code = setcode1;
            bool foundO = false;
            auto &card2 = *it.second.get();

            if(card2.type & Const::TYPE_TOKEN)
            {
                continue;
            }

            while(set_code)
            {
                quint64 setcode2 = card2.setcode;
                quint64 settype = set_code & 0x0fff;
                bool found = false;
                while(setcode2)
                {
                    if((setcode2 & 0x0fff) == settype)
                    {
                        found = true;
                        break;
                    }
                    setcode2 = setcode2 >> 16;
                }

                if(found)
                {
                    foundO = true;
                    break;
                }
                set_code = set_code >> 16;
            }
            if(foundO)
            {
                ls->append(card2.id);
            }
        }
        if(!ls->empty())
        {
            qSort(ls->begin(), ls->end(), idCompare);
            emit result(ls);
        }
    }, cardPool->getCard(id));
}

void CardFilter::searchAll()
{
    auto &map = cardPool->getMap();
    typedef std::remove_reference<decltype(map)>::type T;
    search(KeysIter<T>(map.begin()), KeysIter<T>(map.end()));
}

void CardFilter::searchThis()
{
    auto &t = getCurrent();
    search(t.begin(), t.end());
}

void CardFilter::searchDeck()
{
    auto deck = getDeck();
    search(deck->begin(), deck->end());
}

template<typename T>

void CardFilter::search(const T &begin, const T &end)
{
    auto ls = Type::DeckP::create();
    quint32 type = cardType->currentData().toUInt();
    quint32 subtype = cardTypeSub->currentData().toUInt();
    quint32 race = cardRace->currentData().toUInt();
    quint32 attr = cardAttr->currentData().toUInt();

    auto passPred = rangeMatcher(passEdit->text());
    auto atkPred = rangeMatcher(atkEdit->text());
    auto defPred = rangeMatcher(defEdit->text());
    auto levelPred = rangeMatcher(levelEdit->text());
    auto rankPred = rangeMatcher(rankEdit->text());
    auto scalePred = rangeMatcher(scaleEdit->text());

    quint32 category = 0;

    quint32 unit = 1;
    for(int i : range(32))
    {
        if(effects[i]->isChecked())
        {
            category |= unit;
        }
        unit = unit << 1;
    }

    int limitC = limit->currentData().toInt();
    quint32 otC = ot->currentData().toUInt();

    if(type == Const::TYPE_SPELL || type == Const::TYPE_TRAP)
    {
        subtype |= type;
    }

    quint64 set_code = setEdit->text().toULongLong(nullptr, 16);
    QString name = nameEdit->text();
    bool expr = false;
    QList<QStringList> exprs;
    if(name.indexOf('|') >= 0 || name.indexOf('&') >= 0)
    {
        expr = true;
        QStringList orExprs = name.split("|");
        foreach(auto &orExpr, orExprs)
        {
            exprs.append(orExpr.split("&"));
        }
    }

    for(auto it = begin; it != end; ++it)
    {
        call_with_ref([&](Card& card) {

            if(card.type & Const::TYPE_TOKEN)
            {
                return;
            }

            if(type != ~0U && !(type & card.type))
            {
                return;
            }

            if(limitC != -1 && limitCards->getLimit(card.id) != limitC)
            {
                return;
            }

            if(otC != 0 && (card.ot & 3) != otC)
            {
                return;
            }

            if(!passPred(card.id))
            {
                return;
            }
            if(type & Const::TYPE_MONSTER)
            {
                if(subtype != ~0U && !((subtype & card.type) == subtype))
                {
                    return;
                }

                if(race != ~0U && !(race & card.race))
                {
                    return;
                }

                if(attr != ~0U && !(attr & card.attribute))
                {
                    return;
                }

                if(!atkPred(card.atk))
                {
                    return;
                }

                if(!defPred(card.def))
                {
                    return;
                }

                if(levelPred.isValid())
                {
                    if(!(card.type & Const::TYPE_MONSTER))
                    {
                        return;
                    }
                    if((card.type & Const::TYPE_XYZ) || !levelPred(card.level))
                    {
                        return;
                    }
                }

                if(rankPred.isValid())
                {
                    if(!(card.type & Const::TYPE_MONSTER))
                    {
                        return;
                    }
                    if(!(card.type & Const::TYPE_XYZ) || !rankPred(card.level))
                    {
                        return;
                    }
                }

                if(scalePred.isValid())
                {
                    if(!(card.type & Const::TYPE_MONSTER))
                    {
                        return;
                    }
                    if(!(card.type & Const::TYPE_PENDULUM) || !scalePred(card.scale))
                    {
                        return;
                    }
                }
            }
            else
            {
                if(subtype != ~0U && card.type != subtype)
                {
                    return;
                }
            }

            if((category & card.category) != category)
            {
                return;
            }

            if(set_code)
            {
                quint64 setcode = card.setcode;
                quint64 settype = set_code & 0x0fff;
                quint64 setsubtype = set_code & 0xf000;
                bool found = false;
                while(setcode)
                {
                    if((setcode & 0x0fff) == settype &&
                            (setsubtype == 0 || (setcode & 0xf000) == setsubtype))
                    {
                        found = true;
                        break;
                    }
                    setcode = setcode >> 16;
                }
                if(!found)
                {
                    return;
                }
            }
            if(!name.isEmpty())
            {
                if(!expr)
                {
                    if(card.name.indexOf(name) < 0 &&
                            card.effect.indexOf(name) < 0)
                    {
                        return;
                    }
                }
                else
                {
                    bool outer = false;
                    foreach(auto &andExprs, exprs)
                    {
                        bool inner = true;
                        foreach(auto &str, andExprs)
                        {
                            if(card.name.indexOf(str) < 0 &&
                                    card.effect.indexOf(str) < 0)
                            {
                                inner = false;
                                break;
                            }
                        }
                        if(inner)
                        {
                            outer = true;
                            break;
                        }
                    }
                    if(!outer)
                    {
                        return;
                    }
                }
            }

            ls->append(card.id);
        }, cardPool->getCard(*it));
    }


    qSort(ls->begin(), ls->end(), idCompare);

    emit result(ls);

    return;
}

CardFilter::Matcher CardFilter::rangeMatcher(QString str)
{
    static Matcher ignore([](int) {return true;}, false);
    if(str.isNull() || str.isEmpty())
    {
        return ignore;
    }
    if(str.startsWith("?"))
    {
        return [](int num) {return num == -2;};
    }
    else if(str.startsWith(">="))
    {
        str = str.mid(2);
        bool ok;
        int n = str.toInt(&ok);
        return ok ? [=](int num) {return num >= n;} : ignore;
    }
    else if(str.startsWith(">"))
    {
        str = str.mid(1);
        bool ok;
        int n = str.toInt(&ok);
        return ok? [=](int num) {return num > n;} : ignore;
    }
    else if(str.startsWith("<="))
    {
        str = str.mid(2);
        bool ok;
        int n = str.toInt(&ok);
        return ok? [=](int num) {return num <= n;} : ignore;
    }
    else if(str.startsWith("<"))
    {
        str = str.mid(1);
        bool ok;
        int n = str.toInt(&ok);
        return ok? [=](int num) {return num < n;} : ignore;
    }
    else if(str.startsWith("!="))
    {
        str = str.mid(2);
        bool ok;
        int n = str.toInt(&ok);
        return ok? [=](int num) {return num != n;} : ignore;
    }
    else
    {
        int pos = str.indexOf('-');
        if(pos >= 0)
        {
            bool ok1, ok2;
            int left = str.left(pos).toInt(&ok1);
            int right = str.mid(pos + 1).toInt(&ok2);
            return ok1 && ok2 ? [=](int num) {return num >= left && num <= right;} : ignore;
        }
        else
        {
            bool ok;
            int n = str.toInt(&ok);
            return ok? [=](int num) {return num == n;} : ignore;
        }
    }
}

void CardFilter::revert()
{
    cardType->setCurrentIndex(0);
    cardTypeSub->setCurrentIndex(0);
    cardRace->setCurrentIndex(0);
    cardAttr->setCurrentIndex(0);


    passEdit->clear();
    atkEdit->clear();
    defEdit->clear();
    levelEdit->clear();
    scaleEdit->clear();
    setEdit->clear();
    nameEdit->clear();

    for(auto effect : effects)
    {
        effect->setChecked(false);
    }
}

CardFilter::~CardFilter()
{

}

