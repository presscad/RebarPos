﻿using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Autodesk.AutoCAD.DatabaseServices;
using OZOZ.RebarPosWrapper;

namespace RebarPosCommands
{
    public partial class DrawBOQForm : Form
    {
        public BOQStyle TableStyle { get { return BOQStyle.GetBOQStyle((string)cbStyle.SelectedItem); } }
        public string TableNote { get { return txtNote.Text; } }
        public string TableHeader { get { return txtHeader.Text; } }
        public string TableFooter { get { return txtFooter.Text; } }
        public int Multiplier { get { return (int)udMultiplier.Value; } }
        public bool HideMissing { get { return chkHideMissing.Checked; } }
        public double TextHeight { get { return double.Parse(txtTextHeight.Text); } }
        public int TableRows { get { return int.Parse(txtTableRows.Text); } }
        public double TableMargin { get { return double.Parse(txtTableMargin.Text); } }
        public BOQTable.DrawingUnits DisplayUnit { get { return cbDisplayUnit.SelectedIndex == 0 ? BOQTable.DrawingUnits.Millimeter : BOQTable.DrawingUnits.Centimeter; } }
        public int Precision { get { return cbPrecision.SelectedIndex; } }

        public DrawBOQForm()
        {
            InitializeComponent();
        }

        public bool Init()
        {
            foreach (string style in BOQStyle.GetAllBOQStyles())
            {
                cbStyle.Items.Add(style);
            }

            // Read from settings
            cbStyle.SelectedIndex = Properties.Settings.Default.DrawBOQ_TableStyle;
            chkHideMissing.Checked = Properties.Settings.Default.DrawBOQ_HideMissing;
            txtTextHeight.Text = Properties.Settings.Default.DrawBOQ_TextHeight.ToString();
            txtTableMargin.Text = Properties.Settings.Default.DrawBOQ_TableMargin.ToString();
            cbDisplayUnit.SelectedIndex = Properties.Settings.Default.DrawBOQ_DisplayUnit;
            cbPrecision.SelectedIndex = Properties.Settings.Default.DrawBOQ_Precision;

            return true;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            // Save to settings
            Properties.Settings.Default.DrawBOQ_TableStyle = cbStyle.SelectedIndex;
            Properties.Settings.Default.DrawBOQ_HideMissing = chkHideMissing.Checked;
            Properties.Settings.Default.DrawBOQ_TextHeight = double.Parse(txtTextHeight.Text);
            Properties.Settings.Default.DrawBOQ_TableMargin = double.Parse(txtTableMargin.Text);
            Properties.Settings.Default.DrawBOQ_DisplayUnit = cbDisplayUnit.SelectedIndex;
            Properties.Settings.Default.DrawBOQ_Precision = cbPrecision.SelectedIndex;
            Properties.Settings.Default.Save();

            Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void txtTextHeight_Validating(object sender, System.ComponentModel.CancelEventArgs e)
        {
            TextBox txt = sender as TextBox;
            double val = 0;
            if (string.IsNullOrEmpty(txt.Text) || double.TryParse(txt.Text, out val))
            {
                if (val < 0.0001)
                {
                    errorProvider.SetError(txt, "Yazı yüksekliği sıfırdan büyük olmalı.");
                    errorProvider.SetIconAlignment(txt, ErrorIconAlignment.MiddleLeft);
                    e.Cancel = true;
                }
                else
                {
                    errorProvider.SetError(txt, "");
                }
            }
            else
            {
                errorProvider.SetError(txt, "Lütfen bir reel sayı girin.");
                errorProvider.SetIconAlignment(txt, ErrorIconAlignment.MiddleLeft);
                e.Cancel = true;
            }
        }

        private void txtTableRows_Validating(object sender, System.ComponentModel.CancelEventArgs e)
        {
            TextBox txt = sender as TextBox;
            int val = 0;
            if (string.IsNullOrEmpty(txt.Text) || int.TryParse(txt.Text, out val))
            {
                errorProvider.SetError(txt, "");
            }
            else
            {
                errorProvider.SetError(txt, "Lütfen bir tam sayı girin.");
                errorProvider.SetIconAlignment(txt, ErrorIconAlignment.MiddleLeft);
                e.Cancel = true;
            }
        }

        private void txtTableMargin_Validating(object sender, System.ComponentModel.CancelEventArgs e)
        {
            TextBox txt = sender as TextBox;
            double val = 0;
            if (string.IsNullOrEmpty(txt.Text) || double.TryParse(txt.Text, out val))
            {
                errorProvider.SetError(txt, "");
            }
            else
            {
                errorProvider.SetError(txt, "Lütfen bir reel sayı girin.");
                errorProvider.SetIconAlignment(txt, ErrorIconAlignment.MiddleLeft);
                e.Cancel = true;
            }
        }
    }
}
