#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    MenuTextItem::MenuTextItem() { Lines = Array<zSTRING>(); }

    void MenuTextItem::Init() { MenuItem::Init(); }

    void MenuTextItem::Resize()
    {
        MenuItem::Resize();
        RebuildLines();
    }

    void MenuTextItem::RebuildLines()
    {
        Lines.Clear();
        if (Text.Length() == 0 || !View) return;

        const char* src = Text.ToChar();
        if (!src) return;

        zSTRING currentLine;
        zSTRING currentWord;

        auto flushLine = [&]()
        {
            if (currentLine.Length() > 0)
                Lines.InsertEnd(currentLine);
            currentLine.Clear();
        };

        for (const char* p = src; *p; ++p)
        {
            char c = *p;
            char buf[2] = { c, 0 };

            if (c == ' ' || c == '\t' || c == '\n')
            {                
                zSTRING wordWithPunct = currentWord;
                if ((c == '.' || c == ',') && currentWord.Length() > 0)
                    wordWithPunct += buf;

                if (wordWithPunct.Length() > 0)
                {
                    zSTRING testLine = currentLine;
                    if (testLine.Length() > 0) testLine += " ";
                    testLine += wordWithPunct;

                    if (View->FontSize(testLine) > TextWrapIndex)
                    {
                        flushLine();
                        currentLine = wordWithPunct;
                    }
                    else
                    {
                        if (currentLine.Length() > 0) currentLine += " ";
                        currentLine += wordWithPunct;
                    }
                }

                currentWord.Clear();
                if (c == '\n') flushLine();
            }
            else { currentWord += buf; }
        }

        if (currentWord.Length() > 0)
        {
            zSTRING testLine = currentLine;
            if (testLine.Length() > 0) testLine += " ";
            testLine += currentWord;

            if (View->FontSize(testLine) > TextWrapIndex)
            {
                flushLine();
                currentLine = currentWord;
            }
            else
            {
                if (currentLine.Length() > 0) currentLine += " ";
                currentLine += currentWord;
            }
        }

        if (currentLine.Length() > 0) flushLine();
    }

    void MenuTextItem::Draw()
    {
        BaseMenuElement::Draw();
        if (!IsVisible || IsHiden || !Parent || Lines.IsEmpty() || !View) return;

        zCOLOR color = TextColor_Default;
        if (IsSelected && HasBehavior(UiElementBehaviorFlags::Selectable)) color = TextColor_Selected;
        if (IsHovered && HasBehavior(UiElementBehaviorFlags::Hoverable)) color = TextColor_Hovered;
        if (!IsEnabled) color = TextColor_Disabled;

        View->SetFontColor(color);

        int totalHeight = Lines.GetNum() * View->FontY();
        int startY;

        if (VerticalAlign == UiContentAlignEnum::Begin) startY = 0;
        else if (VerticalAlign == UiContentAlignEnum::End) startY = ScreenVBufferSize - totalHeight;
        else startY = static_cast<int>(ScreenHalfVBufferSize - (totalHeight * 0.5f));

        int y = startY;
        for (auto& line : Lines)
        {
            int x;
            if (HorizontalAlign == UiContentAlignEnum::Begin) x = 0;
            else if (HorizontalAlign == UiContentAlignEnum::End) x = ScreenVBufferSize - View->FontSize(line);
            else x = static_cast<int>(ScreenHalfVBufferSize - (View->FontSize(line) * 0.5f));

            View->Print(x, y, line);
            y += View->FontY();
        }
    }

    void MenuTextItem::Update()
    { 
        BaseMenuElement::Update();
        if ((Text.Length() > 0) && (Lines.IsEmpty()))
            RebuildLines();
    }

    MenuTextItem::~MenuTextItem() { Lines.Clear(); }
}