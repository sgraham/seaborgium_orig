// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/test.h"

#include "sg/ui/docking_split_container.h"
#include "sg/ui/docking_workspace.h"

namespace {

class MainDocument : public Dockable {
 public:
  bool CanUndock() const OVERRIDE { return false; }
};

class ContentPane : public Dockable {
 public:
  bool CanUndock() const OVERRIDE { return true; }
};

}  // namespace

class DockingTest : public LeakCheckTest {
};

TEST_F(DockingTest, Creation) {
  DockingWorkspace workspace;
  workspace.SetRoot(new MainDocument);
  EXPECT_FALSE(workspace.GetRoot()->IsContainer());
  // Just checking for no leaks.
}

TEST_F(DockingTest, AddVerticalSplit) {
  DockingWorkspace workspace;
  MainDocument* main = new MainDocument;
  workspace.SetRoot(main);
  ContentPane* pane = new ContentPane;
  main->get_parent()->SplitChild(kVertical, main, pane);
  EXPECT_TRUE(workspace.GetRoot()->IsContainer());
  EXPECT_EQ(main, workspace.GetRoot()->AsDockingSplitContainer()->left());
  EXPECT_EQ(pane, workspace.GetRoot()->AsDockingSplitContainer()->right());
  EXPECT_EQ(0.5, workspace.GetRoot()->AsDockingSplitContainer()->fraction());
  EXPECT_EQ(kVertical,
            workspace.GetRoot()->AsDockingSplitContainer()->direction());
}

TEST_F(DockingTest, AddVerticalSplitOtherOrder) {
  DockingWorkspace workspace;
  MainDocument* main = new MainDocument;
  workspace.SetRoot(main);
  ContentPane* pane = new ContentPane;
  main->get_parent()->SplitChild(kVertical, pane, main);
  EXPECT_TRUE(workspace.GetRoot()->IsContainer());
  EXPECT_EQ(pane, workspace.GetRoot()->AsDockingSplitContainer()->left());
  EXPECT_EQ(main, workspace.GetRoot()->AsDockingSplitContainer()->right());
  EXPECT_EQ(0.5, workspace.GetRoot()->AsDockingSplitContainer()->fraction());
  EXPECT_EQ(kVertical,
            workspace.GetRoot()->AsDockingSplitContainer()->direction());
}

TEST_F(DockingTest, AddHorizontalSplit) {
  DockingWorkspace workspace;
  MainDocument* main = new MainDocument;
  workspace.SetRoot(main);
  ContentPane* pane = new ContentPane;
  main->get_parent()->SplitChild(kHorizontal, main, pane);
  EXPECT_TRUE(workspace.GetRoot()->IsContainer());
  EXPECT_EQ(main, workspace.GetRoot()->AsDockingSplitContainer()->left());
  EXPECT_EQ(pane, workspace.GetRoot()->AsDockingSplitContainer()->right());
  EXPECT_EQ(0.5, workspace.GetRoot()->AsDockingSplitContainer()->fraction());
  EXPECT_EQ(kHorizontal,
            workspace.GetRoot()->AsDockingSplitContainer()->direction());
}

TEST_F(DockingTest, SubSplit) {
  DockingWorkspace workspace;
  MainDocument* main = new MainDocument;
  workspace.SetRoot(main);
  ContentPane* pane1 = new ContentPane;
  ContentPane* pane2 = new ContentPane;
  main->get_parent()->SplitChild(kVertical, main, pane1);
  pane1->get_parent()->SplitChild(kHorizontal, pane1, pane2);
  EXPECT_TRUE(workspace.GetRoot()->IsContainer());
  DockingSplitContainer* root_as_container =
      workspace.GetRoot()->AsDockingSplitContainer();
  EXPECT_EQ(main, root_as_container->left());
  EXPECT_TRUE(root_as_container->right()->IsContainer());
  DockingSplitContainer* subtree =
      root_as_container->right()->AsDockingSplitContainer();
  EXPECT_EQ(pane1, subtree->left());
  EXPECT_EQ(pane2, subtree->right());
  EXPECT_EQ(kHorizontal, subtree->direction());
}

TEST_F(DockingTest, DragRight) {
}
